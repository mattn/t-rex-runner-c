// Copyright 2021 Betamark Pty Ltd. All rights reserved.
// Author: Shlomi Nissan (shlomi@betamark.com)

#include <math.h>

#include "distance_meter.h"
#include "spritesheet.h"
#include "sys/utilities.h"
#include "sys/graphics.h"

typedef struct {
    Point sprite_def;
    Point pos;
    int max_digits;
    int width;
    int height;
    int dest_width;
    int max_score;
    char default_string[10];
} DistanceMeter;

DistanceMeter distance_meter = {
    .pos = {
        .x = 0,
        .y = 10
    },
    .max_digits = MAX_DISTANCE_UNITS,
    .width = 10,
    .height = 13,
    .dest_width = 11,
    .default_string = ""
};

bool achievement = false;
uint32_t last_distance = 0;
uint32_t flash_iterations = 0;
uint32_t flash_timer = 0;

int GetActualDistance(double distance);
void DrawDistanceMeterDigit(int digit_pos, int value);
void ResetXPos();

void InitDistanceMeter() {
    distance_meter.sprite_def = sprite_definitions[TEXT_SPRITE];
    for (int i = 0; i < distance_meter.max_digits; ++i) {
        DrawDistanceMeterDigit(i, 0);
        distance_meter.default_string[i] = '0';
    }
    distance_meter.max_score = pow(10, distance_meter.max_digits) - 1;
    ResetXPos();
}

void ResetXPos() {
    distance_meter.pos.x = WINDOW_WIDTH -
            (distance_meter.dest_width * (distance_meter.max_digits + 1));
}

void DrawDistanceMeterDigit(int digit_pos, int value) {
    int source_x = distance_meter.width * value;
    int source_y = 0;
    int target_x = digit_pos * distance_meter.dest_width;
    int target_y = distance_meter.pos.y;

    source_x += distance_meter.sprite_def.x;
    source_y += distance_meter.sprite_def.y;

    Texture texture = {
        .id = 0,
        .source = {
            .x = source_x,
            .y = source_y,
            .width = distance_meter.width,
            .height = distance_meter.height
        },
        .destination = {
            .x = target_x + distance_meter.pos.x,
            .y = target_y,
            .width = distance_meter.width,
            .height = distance_meter.height
        }
    };
    DrawTexture(&texture);
}

bool UpdateDistanceMeter(uint32_t delta_time, double distance) {
    bool paint = true;
    bool play_sound = false;

    if (!achievement) {
        int actual_distance = GetActualDistance(distance);

        if (actual_distance > distance_meter.max_score) {
            // score has gone beyond the initial digit count
            ++distance_meter.max_digits;
            distance_meter.max_score = distance_meter.max_score * 10 + 9;
            ResetXPos();
        }

        if (actual_distance > 0) {
            if (actual_distance % ACHIEVEMENT_DISTANCE == 0) {
                achievement = true;
                play_sound = true;
            }
        }

        last_distance = actual_distance;
    } else {
        if (flash_iterations <= DISTANCE_FLASH_ITERATIONS) {
            flash_timer += delta_time;
            if (flash_timer < DISTANCE_FLASH_DURATION) {
                paint = false;
            } else if (flash_timer > DISTANCE_FLASH_DURATION * 2) {
                flash_timer = 0;
                ++flash_iterations;
            }
        } else {
            achievement = false;
            flash_iterations = 0;
            flash_timer = 0;
        }
    }

    if (paint) {
        int digits = last_distance;
        for (int i = distance_meter.max_digits - 1; i >= 0; --i) {
            int digit = 0;
            if (digits != 0) {
                digit = digits % 10;
                digits /= 10;
            }
            DrawDistanceMeterDigit(i, digit);
        }
    }

    return play_sound;
}

int GetActualDistance(double distance) {
    return distance ? (int)(distance * DISTANCE_COEFFICIENT) : 0;
}

void SetDistanceMeterHighScore(int high_score) {
    // TODO: impl
}

void ResetDistanceMeter() {
    // TODO: impl
}