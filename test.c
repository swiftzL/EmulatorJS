#include <emscripten.h>
#include <stdio.h>
#include <stdbool.h>

// 全局变量
static double game_time = 0.0;
static int frame_count = 0;
static bool is_main_loop_paused = false;

// 单帧更新（不包含渲染）
EMSCRIPTEN_KEEPALIVE
void update_frame_logic(double delta_time) {
    // 增加游戏时间和帧计数
    game_time += delta_time;
    frame_count++;
    
    // 在这里放置您的游戏逻辑
    // 物理更新、AI、游戏状态等
    
    // 可以添加简单的进度日志
    if (frame_count % 100 == 0) {
        printf("已处理 %d 帧, 游戏时间: %.2f\n", frame_count, game_time);
    }
}

// 主循环函数
void main_loop_function() {
    // 获取时间差
    double delta_time = 1.0 / 60.0; // 假设60FPS
    
    // 更新帧逻辑
    update_frame_logic(delta_time);
    
    // 这里进行渲染
    render_game();
}

// 渲染函数
void render_game() {
    // 渲染逻辑
    // 仅在主循环调用，不在快速帧处理中调用
}

// 暂停主循环
EMSCRIPTEN_KEEPALIVE
void pause_main_loop() {
    if (!is_main_loop_paused) {
        emscripten_pause_main_loop();
        is_main_loop_paused = true;
    }
}

// 恢复主循环
EMSCRIPTEN_KEEPALIVE
void resume_main_loop() {
    if (is_main_loop_paused) {
        emscripten_resume_main_loop();
        is_main_loop_paused = false;
    }
}

// 快速处理多帧（直接在C中实现，不使用JavaScript循环）
EMSCRIPTEN_KEEPALIVE
void process_multiple_frames(int frame_count, double delta_time) {
    int initial_frame = frame_count;
    
    printf("开始处理 %d 帧，初始帧: %d, 游戏时间: %.2f\n", 
           frame_count, initial_frame, game_time);
    
    // 快速连续处理所有帧
    for (int i = 0; i < frame_count; i++) {
        update_frame_logic(delta_time);
        
        // 每处理100帧输出一次进度
        if ((i + 1) % 100 == 0 || i == frame_count - 1) {
            // 使用EM_ASM调用JavaScript进度回调
            EM_ASM({
                if (typeof Module.onProgressUpdate === 'function') {
                    Module.onProgressUpdate($0, $1);
                }
            }, i + 1, frame_count);
        }
    }
    
    int final_frame = frame_count;
    
    printf("已完成 %d 帧处理，当前帧: %d, 游戏时间: %.2f\n", 
           frame_count, final_frame, game_time);
}

// 初始化模拟
EMSCRIPTEN_KEEPALIVE
void init_simulation() {
    // 初始化
    game_time = 0.0;
    frame_count = 0;
    
    // 设置主循环
    emscripten_set_main_loop(main_loop_function, 0, 1);
}

// 获取当前游戏时间
EMSCRIPTEN_KEEPALIVE
double get_game_time() {
    return game_time;
}

// 获取当前帧数
EMSCRIPTEN_KEEPALIVE
int get_frame_count() {
    return frame_count;
}