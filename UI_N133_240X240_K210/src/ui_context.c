#include "context/guicontext.h"

#include "logic/powerup.h"
#include "logic/P1.h"
#include "logic/P2.h"
#include "logic/P3.h"
#include "logic/P4.h"
#include "logic/poweroff.h"
#include "logic/run.h"

/**
 * @brief 定时器回调接口
 * @param millisecond   定时器周期，单位 ms
 * @param count  累计次数
 */
void on_timer(uint32_t millisecond, uint32_t count) {
  typedef void (*Callback)(uint32_t millisecond, uint32_t count);
  const Callback callbacks[] = {
    ui_powerup_on_timer,
    ui_P1_on_timer,
    ui_P2_on_timer,
    ui_P3_on_timer,
    ui_P4_on_timer,
    ui_poweroff_on_timer,
    ui_run_on_timer
  };
  if(gui_context_get_current_page() < sizeof(callbacks)/sizeof(Callback)) {
    (callbacks[gui_context_get_current_page()])(millisecond, count);
  }
}

/**
 * @brief 页面打开回调接口
 * @param page   页面ID
 */
void on_page_open(uint8_t page) {
  typedef void (*Callback)(uint8_t page);
  const Callback callbacks[] = {
    ui_powerup_on_page_open,
    ui_P1_on_page_open,
    ui_P2_on_page_open,
    ui_P3_on_page_open,
    ui_P4_on_page_open,
    ui_poweroff_on_page_open,
    ui_run_on_page_open
  };
  if(gui_context_get_current_page() < sizeof(callbacks)/sizeof(Callback)) {
    (callbacks[page])(page);
  }
}

/**
 * @brief 按钮点击事件回调接口
 * @param button   按钮控件指针
 * @param type     触摸事件类型
 */
void on_button_touch_event(button_t *button, touch_type_e type) {
  typedef void (*Callback)(button_t *button, touch_type_e type);
  const Callback callbacks[] = {
    ui_powerup_on_button_touch_event,
    ui_P1_on_button_touch_event,
    ui_P2_on_button_touch_event,
    ui_P3_on_button_touch_event,
    ui_P4_on_button_touch_event,
    ui_poweroff_on_button_touch_event,
    ui_run_on_button_touch_event
  };
  if(gui_context_get_current_page() < sizeof(callbacks)/sizeof(Callback)) {
    (callbacks[gui_context_get_current_page()])(button, type);
  }
}

/**
 * @brief 进度条回调接口
 * @param progress  进度条控件指针
 * @param val       进度值
 */
void on_progress_changed(progress_t *progress, uint16_t val) {
  typedef void (*Callback)(progress_t *progress, uint16_t val);
  const Callback callbacks[] = {
    ui_powerup_on_progress_changed,
    ui_P1_on_progress_changed,
    ui_P2_on_progress_changed,
    ui_P3_on_progress_changed,
    ui_P4_on_progress_changed,
    ui_poweroff_on_progress_changed,
    ui_run_on_progress_changed
  };
  if(gui_context_get_current_page() < sizeof(callbacks)/sizeof(Callback)) {
    (callbacks[gui_context_get_current_page()])(progress, val);
  }
}

/**
 * @brief 动画播放结束回调接口
 * @param animation  动画控件指针
 */
void on_animation_play_end(animation_t *animation) {
  typedef void (*Callback)(animation_t *animation);
  const Callback callbacks[] = {
    ui_powerup_on_animation_play_end,
    ui_P1_on_animation_play_end,
    ui_P2_on_animation_play_end,
    ui_P3_on_animation_play_end,
    ui_P4_on_animation_play_end,
    ui_poweroff_on_animation_play_end,
    ui_run_on_animation_play_end
  };
  if(gui_context_get_current_page() < sizeof(callbacks)/sizeof(Callback)) {
    (callbacks[gui_context_get_current_page()])(animation);
  }
}
