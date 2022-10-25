#include "mixer_gui.h"
#include "mixer_api.h"
#include "gfx.h"
#include "src/gwin/gwin_class.h"
#include <array>
#include <optional>

static constexpr uint32_t MAX_LINES = 5;

static MixerAPI api;

struct SetVolumeHelper {
  using img_data_t = std::array<uint8_t, 5500>;
  SetVolumeHelper(int line) : line_(line) {
  }

  void init() {
    gdispImageInit(&img_);

    GWidgetInit wi;
    gwinWidgetClearInit(&wi);
    wi.g.show = gFalse;
    wi.g.x = base_x + multiplier;
    wi.g.y = base_y + line_ * multiplier;
    wi.g.height = 32;
    wi.g.width = 32;
    wi.text = "M";
    btn_mute_ = gwinButtonCreate(0, &wi);


    wi.g.x += multiplier;
    wi.text = "-";
    btn_minus_ = gwinButtonCreate(0, &wi);

    wi.text = "+";
    wi.g.x = gdispGetWidth() - base_x - wi.g.width;
    btn_plus_ = gwinButtonCreate(0, &wi);
  }

  void render() {
    if (not volume_) {
      return;
    }

    draw_over_image();
    if (0 != api.load_image(volume_->pid_, img_data_.data(), img_data_.size())) {
      return;
    }
    show_btns();

    gdispImageClose(&img_);
    if (0 != gdispImageOpenMemory(&img_, img_data_.data())) {
      return;
    }

    gdispImageDraw(&img_, base_x, base_y + line_ * multiplier, 32, 32, 0, 0);
  }

  void set_volume(const mixer::ProgramVolume& vol) {
    volume_ = vol;
  }

  void reset() {
    volume_ = std::nullopt;
    hide_btns();
    draw_over_image();
  }

private:
  void hide_btns() {
    gwinHide(btn_mute_);
    gwinHide(btn_minus_);
    gwinHide(btn_plus_);
  }

  void draw_over_image() {
    gdispFillArea(base_x, base_y + line_ * multiplier, 32, 32, GFX_BLACK);
  }

  void show_btns() {
    gwinShow(btn_mute_);
    gwinShow(btn_minus_);
    gwinShow(btn_plus_);
  }

  GHandle btn_plus_;
  GHandle btn_minus_;
  GHandle btn_mute_;
  gdispImage img_;
  img_data_t img_data_;
  const int line_;
  std::optional<mixer::ProgramVolume> volume_;


  static constexpr unsigned base_x = 10;
  static constexpr unsigned base_y = 10;
  static constexpr unsigned multiplier = 40;
};

std::array<SetVolumeHelper, MAX_LINES> gui_objs = { SetVolumeHelper(0), SetVolumeHelper(1), SetVolumeHelper(2),
                                                    SetVolumeHelper(3), SetVolumeHelper(4) };



void mixer_gui_task(ISerial& uart) {
  api.set_uart(&uart);
  gwinSetDefaultStyle(&BlackWidgetStyle, false);
  gwinSetDefaultFont(gdispOpenFont("DejaVuSans12*"));

  for (auto& helper : gui_objs) {
    helper.init();
  }

  while (1) {
    if (not api.changes()) {
      vTaskDelay(500);
      continue;
    }

    if (MixerAPI::ret_t::OK != api.load_volumes() || (not api.get_volumes()[0])) {
      vTaskDelay(500);
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

    vTaskDelay(1000);
  }
}
