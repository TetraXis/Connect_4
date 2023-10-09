#pragma once

enum class slot
{
	empty,
	player_a,
	player_b
};

enum class game_state
{
	not_ready,
	ready,
	ongoing,
	pause,
	player_a_won,
	player_b_won,
	tie,
	player_a_disqualified,
	player_b_disqualified
};

struct board_data
{
	game_state state = game_state::not_ready;
	unsigned int move = 0;
	slot which_players_turn = slot::empty;

	// 7 colums. 0th place is at the bottom
	slot grid[7][6];

	board_data();
};

struct connect_4;

struct game_administrator
{
	connect_4* game = nullptr;

	void pause();
	void unpause();

	virtual void game_set_notify(slot player_position) = 0;
	virtual void game_state_notify(board_data board) = 0;
};

struct player
{
	virtual int make_move() = 0;
	virtual void game_set_notify(slot player_position) = 0;
	virtual void invalid_move_notify() = 0;
	virtual void game_state_notify(board_data board) = 0;
};

struct connect_4
{
	friend struct game_administrator;
	game_administrator* administrator = nullptr;

private:

	// Owns a pointer
	player* player_a = nullptr;
	// Owns a pointer
	player* player_b = nullptr;
	board_data board;
	// Time in seconds. Player can make any amount of attemts during this time
	float max_waiting_time = 1;

	void fetch_moves(player* player_ptr, int* result);

	/// <summary>
	/// Makes the move on the board
	/// </summary>
	/// <param name="move"> - Column number</param>
	/// <param name="next_player"> - Who should be next</param>
	void make_move(int move, slot next_player);

	/// <summary>
	/// Looks at the state of the board and determines which player, if any, won. If both players won, it is a tie
	/// </summary>
	void update_game_state();

	/// <summary>
	/// Runs the game move by move. When paused stops. You can resume the game by calling this method
	/// </summary>
	void game_loop();

public:

	/// <summary>
	/// Sets up the game and runs it fully. If you want to control the flow use `game_administrator`
	/// </summary>
	void start();

	connect_4();
	~connect_4();

	void set_player_a(player* new_player_a);
	void set_player_b(player* new_player_b);

	bool is_move_valid(int move);

	board_data get_board_state();
};