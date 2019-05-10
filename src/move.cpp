#include <bitset>
#include <iostream>

#include "move.h"

namespace chess {

    void printBitset(std::string s, U64 bits){
        std::cout << s << ": " << std::endl;
        for(int i = 0; i < 8; i++){
            U64 rank = (bits >> ((7-i) * 8)) & 255;
            U64 swap = 0;
            for(int j = 0; j < 8; j++){
                swap |= (((rank & (U64(1) << j)) > 0) << (7 - j));
            }
            std::cout << std::bitset<8>(swap) << std::endl;
        }
    }

    /*
    * Move
    */

    void Move::set(U8 from_layer, U8 from_square, U8 to_layer, U8 to_square,
             U8 take_square, U8 en_passant){
        this->from_layer = from_layer;
        this->from_square = from_square;
        this->to_layer = to_layer;
        this->to_square = to_square;
        this->take_square = take_square;
        this->en_passant = en_passant;
    }

    U8 Move::get_from_layer(){
        return from_layer;
    }

    Piece Move::get_from_piece(){
        return Board::get_piece(get_from_layer());
    }

    U8 Move::get_from_square(){
        return from_square;
    }

    U8 Move::get_to_layer(){
        return to_layer;
    }

    Piece Move::get_to_piece(){
        return Board::get_piece(get_to_layer());
    }

    U8 Move::get_to_square(){
        return to_square;
    }

    U8 Move::get_take_square(){
        return take_square;
    }

    U8 Move::get_en_passant(){
        return en_passant;
    }

    bool Move::is_promotion(){
        return from_layer != to_layer;
    }

    void Move::from_long_algebraic(const std::string &m, Board* board){
        // TODO
    }

    std::string Move::to_long_algebraic(){
        return Board::square_to_coordinate(get_from_square())
             + Board::square_to_coordinate(get_to_square());
    }

    /*
    * Move Generator
    */

    MoveGenerator::MoveGenerator(Position* pos){
        position = pos;
        turn = position->get_turn() == Position::WHITE ? 0 : Board::NB_LAYERS / 2;
        opponent = (turn + Board::NB_LAYERS / 2) % Board::NB_LAYERS;
        layer = 0;
        square_mask = ~U64(0);
        seq = 0;
        own_pieces = position->board.board[turn]
            | position->board.board[turn + 1]
            | position->board.board[turn + 2]
            | position->board.board[turn + 3]
            | position->board.board[turn + 4]
            | position->board.board[turn + 5];
        opponent_pieces = position->board.board[opponent]
            | position->board.board[opponent + 1]
            | position->board.board[opponent + 2]
            | position->board.board[opponent + 3]
            | position->board.board[opponent + 4]
            | position->board.board[opponent + 5];
        all_pieces = own_pieces & opponent_pieces;
        free_square = ~all_pieces;
    }

    int MoveGenerator::generate(){
        U64 notOwnPieces = ~own_pieces;
        int layer;
        U64 pieces;
        U64 movebits;

        layer = turn + Board::WHITE_PAWN_LAYER;
        pieces = position->board.board[layer];
        if(pieces) do {
            int from = __builtin_ffsll(pieces) - 1;
            U64 p = U64(1) << from;
            movebits = generate_pawn_pushes(p, position->get_turn(), notOwnPieces);
            generate_move_bitscan(layer, from, movebits);
            movebits = generate_pawn_double_pushes(p, position->get_turn(), notOwnPieces);
            generate_move_bitscan(layer, from, movebits);
            movebits = generate_pawn_attacks(p, position->get_turn(), notOwnPieces);
            generate_move_bitscan(layer, from, movebits & (opponent_pieces | position->en_passant));
        } while (pieces &= pieces - 1); // reset LS1B

        layer = turn + Board::WHITE_KNIGHT_LAYER;
        pieces = position->board.board[layer];
        if(pieces) do {
            int from = __builtin_ffsll(pieces) - 1;
            U64 p = U64(1) << from;
            movebits = generate_knight_attacks(p, notOwnPieces);
            generate_move_bitscan(layer, from, movebits);
        } while (pieces &= pieces - 1); // reset LS1B

        layer = turn + Board::WHITE_BISHOP_LAYER;
        pieces = position->board.board[layer];
        if(pieces) do {
            int from = __builtin_ffsll(pieces) - 1;
            U64 p = U64(1) << from;
            movebits = generate_bishop_attacks(p, notOwnPieces, free_square);
            generate_move_bitscan(layer, from, movebits);
        } while (pieces &= pieces - 1); // reset LS1B

        layer = turn + Board::WHITE_ROOK_LAYER;
        pieces = position->board.board[layer];
        if(pieces) do {
            int from = __builtin_ffsll(pieces) - 1;
            U64 p = U64(1) << from;
            movebits = generate_rook_attacks(p, notOwnPieces, free_square);
            generate_move_bitscan(layer, from, movebits);
        } while (pieces &= pieces - 1); // reset LS1B

        layer = turn + Board::WHITE_QUEEN_LAYER;
        pieces = position->board.board[layer];
        if(pieces) do {
            int from = __builtin_ffsll(pieces) - 1;
            U64 p = U64(1) << from;
            movebits = generate_queen_attacks(p, notOwnPieces, free_square);
            generate_move_bitscan(layer, from, movebits);
        } while (pieces &= pieces - 1); // reset LS1B

        layer = turn + Board::WHITE_KING_LAYER;
        pieces = position->board.board[layer];
        if(pieces) do {
            int from = __builtin_ffsll(pieces) - 1;
            U64 p = U64(1) << from;
            movebits = generate_king_attacks(p, notOwnPieces);
            generate_move_bitscan(layer, from, movebits);
        } while (pieces &= pieces - 1); // reset LS1B

        // TODO generate castling

        return moveList.size();
    }

    void MoveGenerator::generate_move_bitscan(int layer, int from, U64 bits){
        if(bits) do {
            int idx = __builtin_ffsll(bits) - 1;
            Move m;
            //std::cout << "idx: " << idx << " " << bits << std::endl;
            m.set(layer, from, layer, idx, 0, 0);
            if(!ischeck(&m)){
               moveList.push_back(m);
            }
        } while (bits &= bits - 1); // reset LS1B
    }

    bool MoveGenerator::ischeck(Move* m){
        U64 attacks = 0;
        U64 from = U64(1) << m->get_from_square();
        U64 to = U64(1) << m->get_to_square();
        U64 notOpponentPieces = ~opponent_pieces;
        U64 notOwnPieces = ~((own_pieces & ~from) | to);
        U64 free_square = notOwnPieces & notOpponentPieces;
        U64 ourKing = m->get_to_layer() == turn + Board::WHITE_KING_LAYER ? to : position->board.board[turn + Board::WHITE_KING_LAYER];

        attacks |= generate_pawn_attacks(position->board.board[opponent + Board::WHITE_PAWN_LAYER] & ~to, position->get_turn(), notOpponentPieces);
        attacks |= generate_knight_attacks(position->board.board[opponent + Board::WHITE_KNIGHT_LAYER] & ~to, notOpponentPieces);
        attacks |= generate_bishop_attacks((position->board.board[opponent + Board::WHITE_BISHOP_LAYER] | position->board.board[opponent + Board::WHITE_QUEEN_LAYER]) & ~to, notOpponentPieces, free_square);
        attacks |= generate_rook_attacks((position->board.board[opponent + Board::WHITE_ROOK_LAYER] | position->board.board[opponent + Board::WHITE_QUEEN_LAYER]) & ~to, notOpponentPieces, free_square);
        attacks |= generate_king_attacks(position->board.board[opponent + Board::WHITE_KING_LAYER] & ~to, notOpponentPieces);
        // printBitset("from", from);
        // printBitset("to", to);
        // printBitset("notOpponentPieces", notOpponentPieces);
        // printBitset("notOwnPieces", notOwnPieces);
        // printBitset("opponent attacks", attacks);
        return (attacks & ourKing) != 0;
    }

    U64 aFileMask = 0xFEFEFEFEFEFEFEFE;
    U64 hFileMask = 0x7F7F7F7F7F7F7F7F;
    U64 abFileMask = 0xFCFCFCFCFCFCFCFC;
    U64 ghFileMask = 0x3F3F3F3F3F3F3F3F;

    U64 MoveGenerator::generate_pawn_pushes(U64 layer, Color color, U64 notSelf){
        U64 pushes = 0;
        if(color == Position::WHITE){
            pushes = layer << 8;
        }else{
            pushes = layer >> 8;
        }
        return pushes;
    }

    U64 MoveGenerator::generate_pawn_double_pushes(U64 layer, Color color, U64 notSelf){
        U64 pushes = 0;
        if(color == Position::WHITE){
            pushes = (layer & 0x000000000000FF00) << 16;
        }else{
            pushes = (layer & 0x00FF000000000000) >> 16;
        }
        return pushes;
    }

    U64 MoveGenerator::generate_pawn_attacks(U64 layer, Color color, U64 notSelf){
        U64 attacks = 0;
        if(color == Position::WHITE){
            attacks = (layer & hFileMask) << 7
                    | (layer & aFileMask) << 9;
        }else{
            attacks = (layer & hFileMask) >> 7
                    | (layer & aFileMask) >> 9;
        }
        return attacks & notSelf;
    }

    U64 MoveGenerator::generate_knight_attacks(U64 layer, U64 notSelf){
        U64 attacks = (layer & hFileMask) << 17
                     | (layer & aFileMask) << 15
                     | (layer & ghFileMask) << 10
                     | (layer & abFileMask) << 6
                     | (layer & ghFileMask) >> 6
                     | (layer & abFileMask) >> 10
                     | (layer & hFileMask) >> 15
                     | (layer & aFileMask) >> 17;
        return attacks & notSelf;
    }

    U64 MoveGenerator::generate_bishop_attacks(U64 layer, U64 notSelf, U64 free_square){
        U64 attacks = (layer & aFileMask) << 7
                   | (layer & aFileMask) >> 9
                   | (layer & hFileMask) << 9
                   | (layer & hFileMask) >> 7;
        attacks &= notSelf;
        attacks |= expandBishop((attacks & free_square) | layer) & notSelf;
        attacks |= expandBishop(attacks & free_square) & notSelf;
        attacks |= expandBishop(attacks & free_square) & notSelf;
        attacks |= expandBishop(attacks & free_square) & notSelf;
        attacks |= expandBishop(attacks & free_square) & notSelf;
        attacks |= expandBishop(attacks & free_square) & notSelf;
        return attacks;
    }

    U64 MoveGenerator::generate_rook_attacks(U64 layer, U64 notSelf, U64 free_square){
        U64 attacks = layer << 8
                   | layer >> 8
                   | (layer & hFileMask) << 1
                   | (layer & aFileMask) >> 1;
        attacks &= notSelf;
        attacks |= expandRook((attacks & free_square) | layer) & notSelf;
        attacks |= expandRook(attacks & free_square) & notSelf;
        attacks |= expandRook(attacks & free_square) & notSelf;
        attacks |= expandRook(attacks & free_square) & notSelf;
        attacks |= expandRook(attacks & free_square) & notSelf;
        attacks |= expandRook(attacks & free_square) & notSelf;
        return attacks;
    }

    U64 MoveGenerator::generate_queen_attacks(U64 layer, U64 notSelf, U64 free_square){
        U64 attacks = generate_bishop_attacks(layer, notSelf, free_square)
                    | generate_rook_attacks(layer, notSelf, free_square);
        return attacks & notSelf;
    }

    U64 MoveGenerator::generate_king_attacks(U64 layer, U64 notSelf){
        U64 attacks = (layer & aFileMask) << 7
             | (layer & aFileMask) >> 9
             | (layer & hFileMask) << 9
             | (layer & hFileMask) >> 7
             | layer << 8
             | layer >> 8
             | (layer & hFileMask) << 1
             | (layer & hFileMask) >> 1;
        return attacks & notSelf;
    }

    U64 MoveGenerator::expandBishop(U64 layer){
        return (layer & aFileMask & (layer << 7)) << 7
             | (layer & aFileMask & (layer >> 9)) >> 9
             | (layer & hFileMask & (layer << 9)) << 9
             | (layer & hFileMask & (layer >> 7)) >> 7;
    }

    U64 MoveGenerator::expandRook(U64 layer){
        return (layer & layer << 8) << 8
             | (layer & layer >> 8) >> 8
             | (layer & hFileMask & layer << 1) << 1
             | (layer & aFileMask & layer >> 1) >> 1;
    }

}
