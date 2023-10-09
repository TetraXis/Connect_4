#include "connect_4.h"
#include "timer.h"
#include <thread>
#include <chrono>

#define COLUMNS 7
#define ROWS 6

board_data::board_data()
{
	for (int column = 0; column < COLUMNS; column++)
	{
		for (int row = 0; row < ROWS; row++)
		{
			grid[column][row] = slot::empty;
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

void connect_4::make_move(int move, slot next_player)
{
	if (!is_move_valid(move) || board.which_players_turn == slot::empty)
	{
		return;
	}
	for (int row = 0; row < ROWS; row++)
	{
		if (board.grid[move][row] == slot::empty)
		{
			board.grid[move][row] = board.which_players_turn;
			board.which_players_turn = next_player;
			board.move++;
			break;
		}
	}
	update_game_state();
}

void connect_4::update_game_state()
{
	bool player_a_won = false;
	bool player_b_won = false;

	// Checking all horizontal lines
	for (int row = 0; row < ROWS; row++)
	{
		for (int column = 0; column < COLUMNS - 4; column++)
		{
			if
			(	
				!player_a_won &&
				board.grid[column    ][row] == slot::player_a &&
				board.grid[column + 1][row] == slot::player_a &&
				board.grid[column + 2][row] == slot::player_a &&
				board.grid[column + 3][row] == slot::player_a
			)
			{
				player_a_won = true;
			}
			if
			(
				!player_b_won &&
				board.grid[column    ][row] == slot::player_b &&
				board.grid[column + 1][row] == slot::player_b &&
				board.grid[column + 2][row] == slot::player_b &&
				board.grid[column + 3][row] == slot::player_b
			)
			{
				player_b_won = true;
			}
		}
	}

	// Checking all vertical lines
	for (int column = 0; column < COLUMNS; column++)
	{
		for (int row = 0; row < ROWS - 4; row++)
		{
			if
			(
				!player_a_won &&
				board.grid[column][row    ] == slot::player_a &&
				board.grid[column][row + 1] == slot::player_a &&
				board.grid[column][row + 2] == slot::player_a &&
				board.grid[column][row + 3] == slot::player_a
			)
			{
				player_a_won = true;
			}
			if
			(
				!player_b_won &&
				board.grid[column][row    ] == slot::player_b &&
				board.grid[column][row + 1] == slot::player_b &&
				board.grid[column][row + 2] == slot::player_b &&
				board.grid[column][row + 3] == slot::player_b
			)
			{
				player_b_won = true;
			}
		}
	}

	// Cheking all diagonal lines
	for (int column = 0; column < COLUMNS - 4; column++)
	{
		for (int row = 0; row < ROWS - 4; row++)
		{
			if
			(
				!player_a_won &&
				board.grid[column    ][row    ] == slot::player_a &&
				board.grid[column + 1][row + 1] == slot::player_a &&
				board.grid[column + 2][row + 2] == slot::player_a &&
				board.grid[column + 3][row + 3] == slot::player_a
			)
			{
				player_a_won = true;
			}
			if
			(
				!player_b_won &&
				board.grid[column    ][row    ] == slot::player_b &&
				board.grid[column + 1][row + 1] == slot::player_b &&
				board.grid[column + 2][row + 2] == slot::player_b &&
				board.grid[column + 3][row + 3] == slot::player_b
			)
			{
				player_b_won = true;
			}
		}
	}
	for (int column = 0; column < COLUMNS - 4; column++)
	{
		for (int row = 4; row < ROWS; row++)
		{
			if
			(
				!player_a_won &&
				board.grid[column    ][row    ] == slot::player_a &&
				board.grid[column + 1][row - 1] == slot::player_a &&
				board.grid[column + 2][row - 2] == slot::player_a &&
				board.grid[column + 3][row - 3] == slot::player_a
			)
			{
				player_a_won = true;
			}
			if
			(
				!player_b_won &&
				board.grid[column    ][row    ] == slot::player_b &&
				board.grid[column + 1][row - 1] == slot::player_b &&
				board.grid[column + 2][row - 2] == slot::player_b &&
				board.grid[column + 3][row - 3] == slot::player_b
			)
			{
				player_b_won = true;
			}
		}
	}


	if (!player_a_won || !player_b_won)	// None won
	{
		if (administrator)
		{
			administrator->game_state_notify(board);
		}
		return;
	}

	if (player_a_won && player_b_won)	// Both won
	{
		board.state = game_state::tie;
		player_a->game_state_notify(board);
		player_b->game_state_notify(board);
		if (administrator)
		{
			administrator->game_state_notify(board);
		}
		return;
	}

	if (player_a_won)					// player_a won
	{
		board.state = game_state::player_a_won;
		player_a->game_state_notify(board);
		player_b->game_state_notify(board);
		if (administrator)
		{
			administrator->game_state_notify(board);
		}
		return;
	}

	if (player_b_won)					// player_b won
	{
		board.state = game_state::player_b_won;
		player_a->game_state_notify(board);
		player_b->game_state_notify(board);
		if (administrator)
		{
			administrator->game_state_notify(board);
		}
		return;
	}
}

void connect_4::game_loop()
{

	board.state = game_state::ongoing;

	timer move_timer;
	int choice;
	player* player;
	slot next_player;

	while (board.state == game_state::ongoing)
	{
		choice = -1;
		if (board.which_players_turn == slot::player_a)
		{
			player = player_a;
			next_player = slot::player_b;
		}
		else if (board.which_players_turn == slot::player_b)
		{
			player = player_b;
			next_player = slot::player_a;
		}
		else
		{
			board.state = game_state::not_ready;
			if (administrator)
			{
				administrator->game_state_notify(board);
			}
			break;
		}

		move_timer.start();
		while (move_timer.elapsed_seconds() < max_waiting_time)
		{
			std::thread move_thread(&connect_4::fetch_moves, this, player, &choice);
			if (is_move_valid(choice))
			{
				break;
			}
		}
		//std::terminate(move_thread);
		move_timer.stop();
		make_move(choice, next_player);
	}
}

void connect_4::start()
{
	board.state = game_state::not_ready;

	// Setting players
	if (!player_a || !player_b)
	{
		return;
	}
	player_a->game_set_notify(slot::player_a);
	player_b->game_set_notify(slot::player_b);

	// Setting the board
	board.move = 0;
	board.which_players_turn = slot::player_a;
	for (int column = 0; column < COLUMNS; column++)
	{
		for (int row = 0; row < ROWS; row++)
		{
			board.grid[column][row] = slot::empty;
		}
	}
	board.state = game_state::ready;
	if (administrator)
	{
		administrator->game_state_notify(board);
	}

	game_loop();
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
	return move >= 0 && move < COLUMNS && board.grid[move][ROWS - 1] == slot::empty;
}

board_data connect_4::get_board_state()
{
	return board;
}
