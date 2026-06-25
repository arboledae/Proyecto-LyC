#include "parser.tab.h"
#include "tokens.h"

const char* nombre_token(int token) {
    switch (token) {
        case KW_INICIO:          return "KW_INICIO";
        case KW_SINO:            return "KW_SINO";
        case KW_CUANDO:          return "KW_CUANDO";
        case KW_LOOP:            return "KW_LOOP";
        case KW_IMPRIMIR:        return "KW_IMPRIMIR";
        case KW_RETORNAR:        return "KW_RETORNAR";
        case KW_NUM:             return "KW_NUM";
        case KW_DEC:             return "KW_DEC";
        case KW_TEXT:            return "KW_TEXT";
        case LIT_BOOLEANO:       return "LIT_BOOLEANO";
        case LIT_ENTERO:         return "LIT_ENTERO";
        case LIT_DECIMAL:        return "LIT_DECIMAL";
        case LIT_TEXT:           return "LIT_TEXT";
        case IDENTIFICADOR:      return "IDENTIFICADOR";
        case OP_ASIGNACION:      return "OP_ASIGNACION";
        case OP_SUMA:            return "OP_SUMA";
        case OP_RESTA:           return "OP_RESTA";
        case OP_MULT:            return "OP_MULT";
        case OP_DIV:             return "OP_DIV";
        case OP_MOD:             return "OP_MOD";
        case OP_IGUAL:           return "OP_IGUAL";
        case OP_DIFERENTE:       return "OP_DIFERENTE";
        case OP_MAYOR:           return "OP_MAYOR";
        case OP_MENOR:           return "OP_MENOR";
        case OP_MAYOR_IGUAL:     return "OP_MAYOR_IGUAL";
        case OP_MENOR_IGUAL:     return "OP_MENOR_IGUAL";
        case OP_AND:             return "OP_AND";
        case OP_OR:              return "OP_OR";
        case OP_NOT:             return "OP_NOT";
        case DELIM_PUNTO_COMA:   return "DELIM_PUNTO_COMA";
        case DELIM_PAR_IZQ:      return "DELIM_PAR_IZQ";
        case DELIM_PAR_DER:      return "DELIM_PAR_DER";
        case DELIM_LLAVE_IZQ:    return "DELIM_LLAVE_IZQ";
        case DELIM_LLAVE_DER:    return "DELIM_LLAVE_DER";
        case TOKEN_ERROR:        return "TOKEN_ERROR";
        default:                 return "TOKEN_DESCONOCIDO";
    }
}
