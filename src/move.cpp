#include "move.h"

namespace chess {

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
        // TODO
        return "";
    }

    void Move::from_san(const std::string &m, Board* board){
        // TODO
    }

    std::string Move::to_san(){
        // TODO
        return "";
    }

}
