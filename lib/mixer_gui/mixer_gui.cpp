#include "mixer_gui.h"
#include "mixer_api.h"
#include "gfx.h"

static MixerAPI api;

static void render_volume(const mixer::ProgramVolume&, int line);
static void clear_line(int line);
static void destroy_sliders();


void mixer_gui_task(ISerial& uart) {
  api.set_uart(&uart);
  gwinSetDefaultStyle(&BlackWidgetStyle, false);
  gwinSetDefaultFont(gdispOpenFont("DejaVuSans12*"));

  while (1) {
    if (not api.changes()) {
      vTaskDelay(500);
      continue;
    }

    if (MixerAPI::ret_t::OK != api.load_volumes() || (not api.get_volumes()[0])) {
      vTaskDelay(500);
      continue;
    }
    destroy_sliders();
    int i = 0;
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

GHandle slider_handles[2 * 5] = { 0 };
static void create_slider(int line) {
  GWidgetInit wi;
  gwinWidgetClearInit(&wi);
  wi.g.show = gTrue;
  wi.g.x = base_x + multiplier;
  wi.g.y = base_y + line * multiplier;
  wi.g.height = 32;
  wi.g.width = 32;
  wi.text = "+";
  slider_handles[2 * line] = gwinButtonCreate(0, &wi);

  wi.text = "-";
  wi.g.x += 40;
  slider_handles[2 * line + 1] = gwinButtonCreate(0, &wi);
}

static void destroy_sliders() {
  for (unsigned i = 0; i < sizeof(slider_handles) / sizeof(slider_handles[0]); ++i) {
    if (slider_handles[i]) {
      gwinDestroy(slider_handles[i]);
    }
  }
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
  create_slider(line);
  gdispImageDraw(&img, base_x, base_y + line * multiplier, 32, 32, 0, 0);
  gdispImageClose(&img);
}