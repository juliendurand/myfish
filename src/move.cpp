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
        square_mask = ~U64(0);;
        seq = 0;
        own_pieces = position->board.board[turn]
            + position->board.board[turn + 1]
            + position->board.board[turn + 2]
            + position->board.board[turn + 3]
            + position->board.board[turn + 4]
            + position->board.board[turn + 5];
        opponent_pieces = position->board.board[opponent]
            + position->board.board[opponent + 1]
            + position->board.board[opponent + 2]
            + position->board.board[opponent + 3]
            + position->board.board[opponent + 4]
            + position->board.board[opponent + 5];
        all_pieces = own_pieces & opponent_pieces;
        free_square = ~all_pieces;
    }

    int MoveGenerator::generate(){
        return 0;
    }

    bool MoveGenerator::next(Move *m){
        U64 notOwnPieces = ~own_pieces;
        printBitset("Pawns attacks", generate_pawn_attacks(position->board.board[Board::WHITE_PAWN_LAYER], position->get_turn(), notOwnPieces));
        printBitset("Knight attacks", generate_knight_attacks(position->board.board[Board::WHITE_KNIGHT_LAYER], notOwnPieces));
        printBitset("Bishop attacks", generate_bishop_attacks(position->board.board[Board::WHITE_BISHOP_LAYER], notOwnPieces));
        printBitset("Rook attacks", generate_rook_attacks(position->board.board[Board::WHITE_ROOK_LAYER], notOwnPieces));
        printBitset("Queen attacks", generate_queen_attacks(position->board.board[Board::WHITE_QUEEN_LAYER], notOwnPieces));
        printBitset("King attacks", generate_king_attacks(position->board.board[Board::WHITE_KING_LAYER], notOwnPieces));
        std::cout << "Check ? " << ischeck() << std::endl;
        return false;
        move = m;
        while(next_pseudo_legal()){
            if(!ischeck()){
                return true;
            }
        }
        return false;
    }

    bool MoveGenerator::ischeck(){
        U64 attacks = 0;
        U64 notOpponentPieces = ~opponent_pieces;
        attacks |= generate_pawn_attacks(position->board.board[opponent + Board::WHITE_PAWN_LAYER], position->get_turn(), notOpponentPieces);
        attacks |= generate_knight_attacks(position->board.board[opponent + Board::WHITE_KNIGHT_LAYER], notOpponentPieces);
        attacks |= generate_bishop_attacks(position->board.board[opponent + Board::WHITE_BISHOP_LAYER] | position->board.board[opponent + Board::WHITE_QUEEN_LAYER], notOpponentPieces);
        attacks |= generate_rook_attacks(position->board.board[opponent + Board::WHITE_ROOK_LAYER] | position->board.board[opponent + Board::WHITE_QUEEN_LAYER], notOpponentPieces);
        printBitset("opponent attacks", attacks);
        return (attacks & position->board.board[turn + Board::WHITE_KING_LAYER]) != 0;
    }

    bool MoveGenerator::next_pseudo_legal(){
        while(layer < turn + Board::NB_LAYERS / 2){
            switch(turn + layer){
                case Board::WHITE_PAWN_LAYER:
                    if(next_white_pawn()){
                        return true;
                    }
                    break;
                case Board::WHITE_KNIGHT_LAYER:
                    if(next_knight()){
                        return true;
                    }
                    break;
                case Board::WHITE_BISHOP_LAYER:
                    if(next_bishop()){
                        return true;
                    }
                    break;
                case Board::WHITE_ROOK_LAYER:
                    if(next_rook()){
                        return true;
                    }
                    break;
                case Board::WHITE_QUEEN_LAYER:
                    if(next_queen()){
                        return true;
                    }
                    break;
                case Board::WHITE_KING_LAYER:
                    if(next_king()){
                        return true;
                    }
                    break;
                case Board::BLACK_PAWN_LAYER:
                    if(next_black_pawn()){
                        return true;
                    }
                    break;
                case Board::BLACK_KNIGHT_LAYER:
                    if(next_knight()){
                        return true;
                    }
                    break;
                case Board::BLACK_BISHOP_LAYER:
                    if(next_bishop()){
                        return true;
                    }
                    break;
                case Board::BLACK_ROOK_LAYER:
                    if(next_rook()){
                        return true;
                    }
                    break;
                case Board::BLACK_QUEEN_LAYER:
                    if(next_queen()){
                        return true;
                    }
                    break;
                case Board::BLACK_KING_LAYER:
                    if(next_king()){
                        return true;
                    }
                    break;

            }
            layer++;
            square_mask = ~U64(0);
            seq = 0;
        }
        return false;
    }

    bool MoveGenerator::next_white_pawn(){
        std::cout << "next_white_pawn" << std::endl;
        int l = turn + layer;
        int ffs = __builtin_ffs(position->board.board[l] & square_mask);
        if(ffs){
            int square = ffs - 1;
            int squarebit = U64(1) << square;
            switch(seq){
                case 0:
                    seq++;
                    if((square % 8 > 0) && (opponent_pieces & (squarebit << 7))){
                        move->set(l, square, l, square + 7, U8(0), U8(0));
                        return true;
                    }
                case 1:
                    seq++;
                    if(free_square & squarebit << 8){
                        move->set(l, square, l, square + 8, U8(0), U8(0));
                        return true;
                    }
                case 2:
                    seq++;
                    if((square % 8 < 7) && (opponent_pieces & (squarebit << 9))){
                        move->set(l, square, l, square + 9, U8(0), U8(0));
                        return true;
                    }
                case 3:
                    seq = 0;
                    square_mask &= ~squarebit;
                    if((square / 8 == 1) && (free_square & squarebit << 8) && (free_square & squarebit << 16)){
                        move->set(l, square, l, square + 16, U8(0), U8(0));
                        return true;
                    }
                // TODO promotion
                // TODO en passant
            }
        }
        seq = 0;
        square_mask = 0;
        return false;
    }

    bool MoveGenerator::next_black_pawn(){
        std::cout << "next_black_pawn" << std::endl;
        return false;
    }

    bool MoveGenerator::next_knight(){
        std::cout << "next_knight" << std::endl;
        int l = turn + layer;
        int ffs = __builtin_ffs(position->board.board[l] & square_mask);
        if(ffs){
            int square = ffs - 1;
            int squarebit = U64(1) << square;
            int newpos;
            int newposbit;
            switch(seq){
                case 0:
                    seq++;
                    newpos = square + 17;
                    newposbit = U64(1) << newpos;
                    if(!(own_pieces & newposbit) && (square % 8 < 7) && (newpos < 64)){
                        move->set(l, square, l, newpos, U8(0), U8(0));
                        return true;
                    }
                case 1:
                    seq++;
                    newpos = square + 10;
                    newposbit = U64(1) << newpos;
                    if(!(own_pieces & newposbit) && (square % 8 < 6) && (newpos < 64)){
                        move->set(l, square, l, newpos, U8(0), U8(0));
                        return true;
                    }
                case 2:
                    seq++;
                    newpos = square - 10;
                    newposbit = U64(1) << newpos;
                    if(!(own_pieces & newposbit) && (square % 8 < 6) && (newpos >= 0)){
                        move->set(l, square, l, newpos, U8(0), U8(0));
                        return true;
                    }
                case 3:
                    seq++;
                    newpos = square - 17;
                    newposbit = U64(1) << newpos;
                    if(!(own_pieces & newposbit) && (square % 8 < 7) && (newpos >= 0)){
                        move->set(l, square, l, newpos, U8(0), U8(0));
                        return true;
                    }
                case 4:
                    seq++;
                    newpos = square - 15;
                    newposbit = U64(1) << newpos;
                    if(!(own_pieces & newposbit) && (square % 8 > 0) && (newpos >= 0)){
                        move->set(l, square, l, newpos, U8(0), U8(0));
                        return true;
                    }
                case 5:
                    seq++;
                    newpos = square - 6;
                    newposbit = U64(1) << newpos;
                    if(!(own_pieces & newposbit) && (square % 8 > 1) && (newpos >= 0)){
                        move->set(l, square, l, newpos, U8(0), U8(0));
                        return true;
                    }
                case 6:
                    seq++;
                    newpos = square + 6;
                    newposbit = U64(1) << newpos;
                    if(!(own_pieces & newposbit) && (square % 8 > 1) && (newpos < 64)){
                        move->set(l, square, l, newpos, U8(0), U8(0));
                        return true;
                    }
                case 7:
                    seq = 0;
                    square_mask &= ~squarebit;
                    newpos = square + 15;
                    newposbit = U64(1) << newpos;
                    if(!(own_pieces & newposbit) && (square % 8 > 0) && (newpos < 64)){
                        move->set(l, square, l, newpos, U8(0), U8(0));
                        return true;
                    }
            }
        }
        seq = 0;
        square_mask = 0;
        return false;
    }

    bool MoveGenerator::next_bishop(){
        std::cout << "next_bishop" << std::endl;
        return false;
    }

    bool MoveGenerator::next_rook(){
        std::cout << "next_rook" << std::endl;
        return false;
    }

    bool MoveGenerator::next_queen(){
        std::cout << "next_queen" << std::endl;
        return false;
    }

    bool MoveGenerator::next_king(){
        std::cout << "next_white_king" << std::endl;
        return false;
    }

    void MoveGenerator::generate_move_bitscan(int layer, int from, U64 bits){
        if(bits) do {
            int idx = __builtin_ffs(bits) - 1;
            Move m;
            m.set(layer, from, layer, idx, 0, 0);
            moveList.push_back(m);
        } while (bits &= bits - 1); // reset LS1B
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
            pushes = (layer & 0x00FF000000000000) << 16;
        }else{
            pushes = (layer & 0x000000000000FF00) >> 16;
        }
        return pushes;
    }

    U64 MoveGenerator::generate_pawn_en_passant(U64 layer, Color color, U64 notSelf){
        U64 ep = 0;
        if(color == Position::WHITE){
            // TODO
        }else{
            // TODO
        }
        return ep;
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

    U64 MoveGenerator::generate_bishop_attacks(U64 layer, U64 notSelf){
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

    U64 MoveGenerator::generate_rook_attacks(U64 layer, U64 notSelf){
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
    U64 MoveGenerator::generate_queen_attacks(U64 layer, U64 notSelf){
        U64 attacks = generate_bishop_attacks(layer, notSelf) | generate_rook_attacks(layer, notSelf);
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
