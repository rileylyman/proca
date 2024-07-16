// https://stackoverflow.com/questions/72208683/what-standard-libraries-do-i-need-to-link-to-use-raylib-with-windows-and-clang
#include <math.h>
#include <stdio.h>

#include "raylib/include/raylib.h"
#include "raylib/include/raymath.h"

#define BLACK                                                                  \
  (Color) { 50, 55, 59, 255 }
#define GREY                                                                   \
  (Color) { 74, 88, 89, 255 }
#define PINK                                                                   \
  (Color) { 244, 214, 204, 255 }
#define YELLOW                                                                 \
  (Color) { 244, 184, 96, 255 }
#define RED                                                                    \
  (Color) { 200, 62, 77, 255 }

const int LINE_THICKNESS = 1;

typedef struct Joint Joint;
typedef struct Joint {
  Vector2 pos;
  float radius;
  Joint *constraint;
} Joint;

void UpdateJointPos(Joint *joint) {
  Vector2 pos = joint->pos;

  if (joint->constraint) {
    float x = joint->pos.x - joint->constraint->pos.x;
    float y = -(joint->pos.y - joint->constraint->pos.y);
    float r = sqrt(x * x + y * y);
    if (true || r > joint->constraint->radius) {
      float theta = atan(y / x);
      if (x < 0) {
        theta += 3.14;
      }
      // if (theta < 0) {
      //   theta += 6.28;
      // }
      // printf("%f, %f\n", r, theta / 6.28 * 360);

      pos.x = joint->constraint->pos.x + joint->constraint->radius * cos(theta);
      pos.y = joint->constraint->pos.y - joint->constraint->radius * sin(theta);
      joint->pos = pos;
    }
  }
}

void DrawJoint(Joint *joint) {
  DrawRing(joint->pos, joint->radius - LINE_THICKNESS, joint->radius, 0, 360,
           360, PINK);

  const float inner_radius = 10;
  DrawRing(joint->pos, inner_radius - LINE_THICKNESS, inner_radius, 0, 360, 360,
           RED);

  if (joint->constraint) {
    DrawLineV(joint->constraint->pos, joint->pos, ORANGE);
  }
}

void DrawBody(Joint *joint) {
  if (!joint->constraint) {
    return;
  }
  const int steps = 50;

  Vector2 dir =
      Vector2Normalize(Vector2Subtract(joint->constraint->pos, joint->pos));
  Vector2 cross = {-dir.y, dir.x};

  for (int i = 0; i < steps; i++) {
    float t = (float)i / (float)steps;
    Vector2 pos = Vector2Lerp(joint->pos, joint->constraint->pos, t);
    float radius = Lerp(joint->radius, joint->constraint->radius, t);
    Vector2 out1 = Vector2Add(pos, Vector2Scale(cross, radius));
    Vector2 out2 = Vector2Add(pos, Vector2Scale(cross, -radius));
    DrawPixelV(out1, PINK);
    DrawPixelV(out2, PINK);
  }

  DrawBody(joint->constraint);
}

int main() {
  const int screenWidth = 1920;
  const int screenHeight = 1080;

  Joint joints[] = {
      {.pos = {300, 300}, .radius = 50.0, .constraint = NULL},
      {.pos = {200, 100}, .radius = 75.0, .constraint = NULL},
      {.pos = {200, 500}, .radius = 60.0, .constraint = NULL},
      {.pos = {200, 500}, .radius = 50.0, .constraint = NULL},
  };
  joints[1].constraint = &joints[0];
  joints[2].constraint = &joints[1];
  joints[3].constraint = &joints[2];
  const int num_joints = sizeof(joints) / sizeof(Joint);

  SetConfigFlags(FLAG_MSAA_4X_HINT);
  InitWindow(screenWidth, screenHeight, "");

  SetTargetFPS(60);
  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(GREY);

    Vector2 mouse_pos = GetMousePosition();
    joints[0].pos = mouse_pos;

    for (int i = 0; i < num_joints; i++) {
      UpdateJointPos(&joints[i]);
      DrawJoint(&joints[i]);
    }
    DrawJoint(&joints[0]);
    DrawJoint(&joints[num_joints - 1]);

    DrawBody(&joints[num_joints - 1]);

    EndDrawing();
  }

  CloseWindow();
  return 0;
}