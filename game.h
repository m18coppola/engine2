enum PlayerCommandType {
    START_MOVE = 0,
    STOP_MOVE
};

enum PlayerMoveDir {
    UP = 0,
    DOWN,
    LEFT,
    RIGHT,
};

struct PlayerState {
    int xpos;
    int ypos;
    int xvel;
    int yvel;
};

struct PlayerCommand {
    enum PlayerCommandType cmd;
    enum PlayerMoveDir move_dir;
};
