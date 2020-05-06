#include <iostream>
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

    double quiesce(double alpha, double beta, Position* position){
        double stand_pat = eval(position) * (position->get_turn() == Position::WHITE ? 1 : -1); 
        if(stand_pat >= beta){
            return beta;
        }
        if(alpha < stand_pat){
            alpha = stand_pat;
        }
        /*until(){
            if(score >= beta){
                return beta;
            }
            if(score > alpha){
               alpha = score;
            }
        }*/
        return alpha;
    }

	double alphabeta(double alpha, double beta, Position* position, int depth){
		if(depth == 0){
			return quiesce(alpha, beta, position); 
		}

		MoveGenerator generator(position);
		generator.generate();
		double value = -10000;
		for(Move m : generator.moveList){
        	Position new_position(*position);
        	new_position.make_move(&m);
        	value = std::max(value, -alphabeta(-beta, -alpha, &new_position, depth - 1));
        	alpha = std::max(value, alpha);
        	if(alpha >= beta){
        		break;  // cut-off
        	}
        }
        return value;
	}

	std::string search(Position* position, int depth){
		double infinity = 10000000;
		std::string bestMove = "";

		double value = -infinity;

        MoveGenerator generator(position);
		generator.generate();
        for(Move m : generator.moveList){
        	Position new_position(*position);
        	new_position.make_move(&m);
        	
    		double score = -alphabeta(-infinity, infinity, &new_position, depth - 1);// * (position->get_turn() == Position::WHITE ? 1 : -1);
    		if(score >= value){
    			value = score;
    			bestMove = m.to_long_algebraic();
    		}
    		//std::cout << m.to_long_algebraic() << " " << score << std::endl; 
        }
		return bestMove;
	}

}