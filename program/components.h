#ifndef COMPONENTS_H
#define COMPONENTS_H

struct MovementComponent {
    float x;
    float y;
    float speed;
};

typedef enum { YeeHaw = 0, FlushedHaw = 1, HotHaw = 2 } EnemyType;

struct EnemyTypeComponent {
    EnemyType type;
};

struct DistanceComponent {
    float distance;
};

#endif
