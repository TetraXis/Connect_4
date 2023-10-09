#include "connect_4.h"
#include "timer.h"
#include <thread>
#include <chrono>

board_data::board_data()
{
	for (int i = 0; i < 7; i++)
	{
		for (int j = 0; j < 6; j++)
		{
			grid[i][j] = slot::empty;
		}
	}
}

void connect_4::fetch_moves(player* player_ptr, int* result)
{
	int attempts = 0;
	do 
	{
		*result = player_ptr->make_move();
		attempts++;
	} while (!is_move_valid(*result) && attempts < 1000);
}

int connect_4::start()
{
	// Setting players
	if (!player_a || !player_b)
	{
		board.state = game_state::not_ready;
		return -1;
	}
	player_a->game_set_notify(slot::player_a);
	player_b->game_set_notify(slot::player_b);

	// Setting the board
	board.move = 0;
	for (int i = 0; i < 7; i++)
	{
		for (int j = 0; j < 6; j++)
		{
			board.grid[i][j] = slot::empty;
		}
	}
	board.state = game_state::ready;

	// Game loop
	board.state = game_state::ongoing;

	timer move_timer;
	int choice_a, choice_b;

	while (board.state == game_state::ongoing)
	{
		// player_a turn
		choice_a = -1;
		move_timer.start();
		while (move_timer.elapsed_seconds() < max_waiting_time)
		{
			std::thread move_thread(&connect_4::fetch_moves, player_a, &choice_a);
			if (is_move_valid(choice_a))
			{
				break;
			}
		}
		//std::terminate(move_thread);
	}
}

connect_4::connect_4()
{

}

connect_4::~connect_4()
{
	if (player_a)
	{
		delete player_a;
	}
	if (player_b)
	{
		delete player_b;
	}
}

void connect_4::set_player_a(player* new_player_a)
{
	if (board.state == game_state::ongoing || board.state == game_state::pause)
	{
		board.state = game_state::player_a_disqualified;
		return;
	}
	player_a = new_player_a;
}

void connect_4::set_player_b(player* new_player_b)
{
	if (board.state == game_state::ongoing || board.state == game_state::pause)
	{
		board.state = game_state::player_b_disqualified;
		return;
	}
	player_b = new_player_b;
}

bool connect_4::is_move_valid(int move)
{
	return move >= 0 && move <= 7 && board.grid[move][6] == slot::empty;
}

board_data connect_4::get_board_state()
{
	return board;
}
