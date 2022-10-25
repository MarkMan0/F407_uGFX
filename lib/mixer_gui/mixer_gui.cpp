#include "mixer_gui.h"
#include "comm_api.h"
#include "gfx.h"
#include "src/gwin/gwin_class.h"
#include <array>
#include <optional>

static constexpr uint32_t MAX_LINES = 5;

static CommAPI& api = CommAPI::get_instance();

static gFont font;
struct SetVolumeHelper {
  using img_data_t = std::array<uint8_t, 5500>;
  SetVolumeHelper(int line) : line_(line) {
  }

  void init() {
    GWidgetInit wi;
    gwinWidgetClearInit(&wi);
    wi.g.show = gFalse;
    wi.g.x = base_x;
    wi.g.y = base_y + line_ * multiplier;
    wi.g.height = 32;
    wi.g.width = 32;

    img_handle_ = gwinImageCreate(0, &wi.g);
    // gwinSetBgColor(img_handle_, GFX_BLACK);

    wi.g.x += multiplier;
    wi.text = "M";
    btn_mute_ = gwinButtonCreate(0, &wi);


    wi.g.x += multiplier;
    wi.text = "-";
    btn_minus_ = gwinButtonCreate(0, &wi);


    wi.g.x += multiplier;
    wi.g.width = gdispGetWidth() - base_x - 3 * multiplier - base_x - multiplier;
    wi.text = "%";
    slider_ = gwinSliderCreate(0, &wi);

    wi.text = "+";
    wi.g.x = gdispGetWidth() - base_x - 32;
    btn_plus_ = gwinButtonCreate(0, &wi);
  }

  void render() {
    if (not volume_) {
      return;
    }
    auto curr = *volume_;  // needed for debug, pio doesnt work with optional :/

    if (volume_changed_) {
      volume_changed_ = false;
      show_widgets();
      if (curr.muted_) {
        snprintf(slider_txt_.data(), slider_txt_.size() - 1, "Mute (%d%%)", curr.volume_);
      } else {
        snprintf(slider_txt_.data(), slider_txt_.size() - 1, "%d%%", curr.volume_);
      }
      gwinSliderSetPosition(slider_, curr.volume_);
      gwinSetText(slider_, slider_txt_.data(), gFalse);
    }

    if (session_change_) {
      if (0 != api.load_image(curr.pid_, img_data_.data(), img_data_.size())) {
        return;
      }
      gwinClear(img_handle_);
      if (gTrue != gwinImageOpenMemory(img_handle_, img_data_.data())) {
        return;
      }
      // all success, dont redraw next time
      session_change_ = false;
    }
  }

  void set_volume(const mixer::ProgramVolume& vol) {
    if (volume_ && volume_->pid_ == vol.pid_) {
      // picture haven't changed
      // session_change_ = false;
      volume_changed_ = volume_changed_ || (vol.volume_ != volume_->volume_) || (vol.muted_ != volume_->muted_);
    } else {
      session_change_ = true;
      volume_changed_ = true;
    }
    volume_ = vol;
  }

  void reset() {
    volume_ = std::nullopt;
    hide_widgets();
  }

  void handle_event(const GEvent* ev) {
    if (not volume_) {
      return;
    }

    if (ev->type == GEVENT_GWIN_BUTTON) {
      const auto handle = ((GEventGWinButton*)ev)->gwin;
      handle_button_event(handle);
    }

    if (ev->type == GEVENT_GWIN_SLIDER) {
      handle_slider_event((GEventGWinSlider*)ev);
    }
  }

private:
  void hide_widgets() {
    gwinHide(img_handle_);
    gwinHide(btn_mute_);
    gwinHide(btn_minus_);
    gwinHide(btn_plus_);
    gwinHide(slider_);
    gwinHide(img_handle_);
  }

  void show_widgets() {
    gwinShow(img_handle_);
    gwinShow(btn_mute_);
    gwinShow(btn_minus_);
    gwinShow(btn_plus_);
    gwinShow(slider_);
  }

  void handle_button_event(const GHandle h) {
    handle_mute_btn(h);
    handle_plus_minus_btn(h);
  }

  void handle_plus_minus_btn(const GHandle h) {
    if (h != btn_minus_ && h != btn_plus_) {
      return;
    }

    bool need_change = false;

    int16_t vol = volume_->volume_;  // so no underflow when minus
    // finer control for master volume
    const int16_t increment = volume_->pid_ == -1 ? 2 : 10;
    if (h == btn_minus_) {
      need_change = true;
      vol -= increment;
    } else if (h == btn_plus_) {
      need_change = true;
      vol += increment;
    }

    if (need_change) {
      api.set_volume(volume_->pid_, utils::constrain(vol, 0, 100));
    }
  }

  void handle_mute_btn(const GHandle h) {
    if (h != btn_mute_) {
      return;
    }
    api.set_mute(volume_->pid_, not volume_->muted_);
  }

  void handle_slider_event(const GEventGWinSlider* ev) {
    if (ev->gwin == slider_ && ev->action == GSLIDER_EVENT_SET) {
      api.set_volume(volume_->pid_, utils::constrain(ev->position, 0, 100));
    }
  }

  GHandle img_handle_;
  GHandle btn_plus_;
  GHandle btn_minus_;
  GHandle btn_mute_;
  GHandle slider_;
  img_data_t img_data_;
  const int line_;
  std::optional<mixer::ProgramVolume> volume_;
  bool session_change_ = true;
  bool volume_changed_ = false;
  std::array<char, 30> slider_txt_ = { 0 };

  static constexpr unsigned base_x = 10;
  static constexpr unsigned base_y = 10;
  static constexpr unsigned multiplier = 40;
};

std::array<SetVolumeHelper, MAX_LINES> gui_objs = { SetVolumeHelper(0), SetVolumeHelper(1), SetVolumeHelper(2),
                                                    SetVolumeHelper(3), SetVolumeHelper(4) };



void mixer_gui_task() {
  font = gdispOpenFont("DejaVuSans12*");
  gwinSetDefaultStyle(&BlackWidgetStyle, false);
  gwinSetDefaultFont(font);


  GListener gl;
  geventListenerInit(&gl);
  gwinAttachListener(&gl);

  for (auto& helper : gui_objs) {
    helper.init();
  }

  while (1) {
    GEvent* pe = geventEventWait(&gl, 1000);

    if (pe) {
      for (auto& obj : gui_objs) {
        obj.handle_event(pe);
      }
    }

    if (not api.changes()) {
      continue;
    }

    if (CommAPI::ret_t::OK != api.load_volumes() || (not api.get_volumes()[0])) {
      continue;
    }


    unsigned line = 0;
    for (const auto& vol : api.get_volumes()) {
      if (vol) {
        auto& curr = gui_objs[line];
        ++line;
        curr.set_volume(*vol);
        curr.render();
      }
    }
    // clear the rest of them
    for (; line < gui_objs.size(); ++line) {
      gui_objs[line].reset();
    }
  }
}
