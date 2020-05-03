#include <utility> 

#include "search.h"

namespace chess {

	double eval(Position* position){
		float score = 0;
		score += __builtin_popcountl(position->board.board[Board::WHITE_PAWN_LAYER]) * 1;
        score += __builtin_popcountl(position->board.board[Board::WHITE_KNIGHT_LAYER]) * 3;
        score += __builtin_popcountl(position->board.board[Board::WHITE_BISHOP_LAYER]) * 3;
        score += __builtin_popcountl(position->board.board[Board::WHITE_ROOK_LAYER]) * 5;
        score += __builtin_popcountl(position->board.board[Board::WHITE_QUEEN_LAYER]) * 9;
        score += __builtin_popcountl(position->board.board[Board::WHITE_KING_LAYER]) * 1000;
        score += __builtin_popcountl(position->board.board[Board::BLACK_PAWN_LAYER]) * -1;
        score += __builtin_popcountl(position->board.board[Board::BLACK_KNIGHT_LAYER]) * -3;
        score += __builtin_popcountl(position->board.board[Board::BLACK_BISHOP_LAYER]) * -3;
        score += __builtin_popcountl(position->board.board[Board::BLACK_ROOK_LAYER]) * -5;
        score += __builtin_popcountl(position->board.board[Board::BLACK_QUEEN_LAYER]) * -9;
        score += __builtin_popcountl(position->board.board[Board::BLACK_KING_LAYER]) * -1000; 
        return score;
	}

	double minimax(Position* position, int depth){
		double value = -1000000;

		if(depth == 0){
			return eval(position) * (position->get_turn() == Position::WHITE ? 1 : -1);
		}

		MoveGenerator generator(position);
		generator.generate();
		for(Move m : generator.moveList){
        	Position new_position(*position);
        	new_position.make_move(&m);
        	double subtree_value = -minimax(&new_position, depth - 1);
        	value = std::max(value, subtree_value);
        }
        return value;
	}

	std::string search(Position* position, int depth){
		double value = -10000000;
		std::string bestMove = "";

        MoveGenerator generator(position);
		generator.generate();
        for(Move m : generator.moveList){
        	Position new_position(*position);
        	new_position.make_move(&m);
        	double subtree_value = -minimax(&new_position, depth - 1);
        	if(subtree_value > value){
        		value = subtree_value;
        		bestMove = m.to_long_algebraic();
        	}
        }
        
		return bestMove;
	}

}