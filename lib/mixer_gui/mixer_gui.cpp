#include "mixer_gui.h"
#include "mixer_api.h"
#include "gfx.h"
#include "src/gwin/gwin_class.h"

static constexpr uint32_t MAX_LINES = 5;

static MixerAPI api;

static void render_volume(const mixer::ProgramVolume&, int line);
static void clear_line(int line);
static void init_sliders();
static void hide_sliders();
static void show_slider(int line);


void mixer_gui_task(ISerial& uart) {
  api.set_uart(&uart);
  gwinSetDefaultStyle(&BlackWidgetStyle, false);
  gwinSetDefaultFont(gdispOpenFont("DejaVuSans12*"));
  init_sliders();
  while (1) {
    if (not api.changes()) {
      vTaskDelay(500);
      continue;
    }

    if (MixerAPI::ret_t::OK != api.load_volumes() || (not api.get_volumes()[0])) {
      vTaskDelay(500);
      continue;
    }
    int i = 0;
    hide_sliders();
    for (const auto& vol : api.get_volumes()) {
      if (vol) {
        render_volume(*vol, i);
      } else {
        clear_line(i);
      }
      ++i;
    }

    vTaskDelay(1000);
  }
}

static uint8_t img_buff[6000] = { 0 };


constexpr unsigned base_x = 10;
constexpr unsigned base_y = 10;
constexpr unsigned multiplier = 40;

static void clear_line(int line) {
  gdispFillArea(0, base_y + line * multiplier, gdispGetWidth(), multiplier, GFX_BLACK);
}

static GHandle slider_handles[MAX_LINES][2];

static void init_sliders() {
  for (unsigned i = 0; i < MAX_LINES; ++i) {
    GWidgetInit wi;
    gwinWidgetClearInit(&wi);
    wi.g.show = gFalse;
    wi.g.x = base_x + multiplier;
    wi.g.y = base_y + i * multiplier;
    wi.g.height = 32;
    wi.g.width = 32;
    wi.text = "+";
    slider_handles[i][0] = gwinButtonCreate(0, &wi);

    wi.text = "-";
    wi.g.x += 40;
    slider_handles[i][1] = gwinButtonCreate(0, &wi);
  }
}

static void hide_sliders() {
  for (auto& btns : slider_handles) {
    btns[0]->flags &= ~(GWIN_FLG_VISIBLE | GWIN_FLG_SYSVISIBLE);
    btns[0]->flags |= (GWIN_FLG_NEEDREDRAW | GWIN_FLG_BGREDRAW);

    btns[1]->flags &= ~(GWIN_FLG_VISIBLE | GWIN_FLG_SYSVISIBLE);
    btns[1]->flags |= (GWIN_FLG_NEEDREDRAW | GWIN_FLG_BGREDRAW);
  }
}


static void show_slider(int line) {
  if (not utils::within(line, 0, 4)) {
    return;
  }
  gwinShow(slider_handles[line][0]);
  gwinShow(slider_handles[line][1]);
}


static void render_volume(const mixer::ProgramVolume& vol, int line) {
  gdispImage img;
  gdispImageInit(&img);
  img.width = 32;
  img.height = 32;

  if (MixerAPI::ret_t::OK != api.load_image(vol.pid_, img_buff, sizeof(img_buff))) {
    clear_line(line);
  }
  gdispImageOpenMemory(&img, img_buff);

  clear_line(line);
  show_slider(line);
  gdispImageDraw(&img, base_x, base_y + line * multiplier, 32, 32, 0, 0);
  gdispImageClose(&img);
}