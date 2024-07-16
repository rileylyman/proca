// https://stackoverflow.com/questions/72208683/what-standard-libraries-do-i-need-to-link-to-use-raylib-with-windows-and-clang
#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>

#include "raylib/include/raylib.h"
#include "raylib/include/raymath.h"

#define ONYX                                                                  \
  (Color) { 50, 55, 59, 255 }
#define GREY                                                                   \
  (Color) { 74, 88, 89, 255 }
#define SAKURA                                                                   \
  (Color) { 244, 214, 204, 255 }
#define POPPY                                                                 \
  (Color) { 244, 184, 96, 255 }
#define CHERRY                                                                    \
  (Color) { 200, 62, 77, 255 }

const int LINE_THICKNESS = 1;
const float SPEED_PPS = 300;

typedef struct Joint Joint;
typedef struct Joint {
  Vector2 pos;
  float radius;
} Joint;

void UpdateJointPos(Joint *joint, Joint* constraint) {
  assert(joint && constraint);
  Vector2 pos = joint->pos;

  float x = joint->pos.x - constraint->pos.x;
  float y = -(joint->pos.y - constraint->pos.y);
  float r = sqrt(x * x + y * y);
  if (true || r > constraint->radius) {
    float theta = atan(y / x);
    if (x < 0) {
      theta += 3.14;
    }
    // if (theta < 0) {
    //   theta += 6.28;
    // }
    // printf("%f, %f\n", r, theta / 6.28 * 360);

    pos.x = constraint->pos.x + constraint->radius * cos(theta);
    pos.y = constraint->pos.y - constraint->radius * sin(theta);
    joint->pos = pos;
  }
}

void DrawJoint(Joint *joint, Joint* constraint) {
  assert(joint);
  DrawRing(joint->pos, joint->radius - LINE_THICKNESS, joint->radius, 0, 360,
           360, SAKURA);

  const float inner_radius = 10;
  DrawRing(joint->pos, inner_radius - LINE_THICKNESS, inner_radius, 0, 360, 360,
           CHERRY);

  if (constraint) {
    DrawLineV(constraint->pos, joint->pos, POPPY);
  }
}

typedef struct Spine {
  Joint* joints;
  int num_joints;
} Spine;


void DrawBody(Spine *spine, bool draw_joints) {
  if (draw_joints) {
    DrawJoint(&spine->joints[0], NULL);
    for (int i = 1; i < spine->num_joints; i++) {
      DrawJoint(&spine->joints[i], &spine->joints[i - 1]);
    }
  }
}

void UpdateSpinePos(Spine* spine, Vector2 target_pos) {
  float delta_time = GetFrameTime();
  if (Vector2Distance(spine->joints[0].pos, target_pos)) {
    spine->joints[0].pos = Vector2MoveTowards(spine->joints[0].pos, target_pos, SPEED_PPS * delta_time);

  }
  for (int i = 1; i < spine->num_joints; i++) {
    UpdateJointPos(&spine->joints[i], &spine->joints[i - 1]);
  }
}

int main() {
  const int screenWidth = 1920;
  const int screenHeight = 1080;

  Joint joints[] = {
      {.pos = {300, 300}, .radius = 50.0},
      {.pos = {200, 100}, .radius = 75.0},
      {.pos = {200, 500}, .radius = 60.0},
      {.pos = {200, 500}, .radius = 50.0},
  };
  const int num_joints = sizeof(joints) / sizeof(Joint);
  Spine spine = {.joints = joints, .num_joints = num_joints};

  SetConfigFlags(FLAG_MSAA_4X_HINT);
  InitWindow(screenWidth, screenHeight, "");

  Vector2 target_pos = joints[0].pos;

  SetTargetFPS(60);
  while (!WindowShouldClose()) {
    if (true || IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
      target_pos = GetMousePosition();
    }
    UpdateSpinePos(&spine, target_pos);

    BeginDrawing();
    ClearBackground(GREY);

    DrawBody(&spine, true);

    EndDrawing();
  }

  CloseWindow();
  return 0;
}