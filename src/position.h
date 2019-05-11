#ifndef POSITION_H_INCLUDED
#define POSITION_H_INCLUDED

#include <cstdint>

#include "move.h"
#include "utils.h"

namespace chess{

    class Board{
    public:
        static const Piece EMPTY = ' ';
        static const Piece WHITE_PAWN = 'P';
        static const Piece WHITE_KNIGHT ='N';
        static const Piece WHITE_BISHOP ='B';
        static const Piece WHITE_ROOK ='R';
        static const Piece WHITE_QUEEN ='Q';
        static const Piece WHITE_KING ='K';
        static const Piece BLACK_PAWN ='p';
        static const Piece BLACK_KNIGHT ='n';
        static const Piece BLACK_BISHOP ='b';
        static const Piece BLACK_ROOK ='r';
        static const Piece BLACK_QUEEN = 'q';
        static const Piece BLACK_KING = 'k';

        static const int INVALID_LAYER = -1;
        static const int WHITE_PAWN_LAYER = 0;
        static const int WHITE_KNIGHT_LAYER = 1;
        static const int WHITE_BISHOP_LAYER = 2;
        static const int WHITE_ROOK_LAYER = 3;
        static const int WHITE_QUEEN_LAYER = 4;
        static const int WHITE_KING_LAYER = 5;
        static const int BLACK_PAWN_LAYER = 6;
        static const int BLACK_KNIGHT_LAYER = 7;
        static const int BLACK_BISHOP_LAYER = 8;
        static const int BLACK_ROOK_LAYER = 9;
        static const int BLACK_QUEEN_LAYER = 10;
        static const int BLACK_KING_LAYER = 11;

        static const int NB_LAYERS = 12;

        static const Piece PIECES[12];

        U64 board[NB_LAYERS];

        Board();
        void set_square(Piece piece, int square);
        Piece get_square(int square);
        void empty();
        static Piece get_piece(int layer);
        static int get_layer(Piece piece);
        static int coordinates_to_square(std::string coordinates);
        static std::string square_to_coordinate(int square);
        void make_move(Move* move);
        void print();

    private:
        U64 get_bitmask(int square);
    };

    class Position{

        friend class MoveGenerator;

    private:
        Board board;
        U8 castling;
        U8 en_passant;
        int plies;
        int reversible_plies;

    public:
        static const U8 WHITE_KINGSIDE_CAST  = U8(1) << 0;
        static const U8 WHITE_QUEENSIDE_CAST = U8(1) << 1;
        static const U8 BLACK_KINGSIDE_CAST  = U8(1) << 2;
        static const U8 BLACK_QUEENSIDE_CAST = U8(1) << 3;

        static const Color WHITE = 'w';
        static const Color BLACK = 'b';

        Position();
        void set_start_position();
        std::string export_fen();
        void import_fen(const std::string &fen);
        Color get_turn();
        void set_turn(Color color);
        void set_castling(Piece cast_type, bool right);
        bool get_castling(Piece cast_type);
        void set_en_passant(std::string ep);
        std::string get_en_passant();
        std::string get_all_castling();
        int get_move();
        int get_plies();
        int get_reversible_plies();
        std::string get_square(int square);
        void reset();
        void make_move(Move* move);
        Move get_move_from_long_algebraic(const std::string &m);
    };

}

#endif // #ifndef POSITION_H_INCLUDED

