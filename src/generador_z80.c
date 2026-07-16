#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codigo_intermedio.h"
#include "generador_z80.h"

/* ── Pool de cadenas (deduplicadas) ─────────────────────────── */
static char **strs   = NULL;
static int  n_strs   = 0;
static int  cap_strs = 0;

static int str_indice(const char *lit) {
    for (int i = 0; i < n_strs; i++)
        if (strcmp(strs[i], lit) == 0) return i;
    if (n_strs == cap_strs) {
        cap_strs = cap_strs ? cap_strs * 2 : 16;
        strs = (char**)realloc(strs, cap_strs * sizeof(char*));
    }
    strs[n_strs] = strdup(lit);
    return n_strs++;
}

static void liberar_strs(void) {
    for (int i = 0; i < n_strs; i++) free(strs[i]);
    free(strs);
    strs = NULL; n_strs = cap_strs = 0;
}

/* ── Clasificacion de operandos del TAC ─────────────────────── */
static int es_cadena(const char *s)  { return s && s[0] == '"'; }

static int es_entero(const char *s) {
    if (!s || !*s) return 0;
    for (const char *p = s; *p; p++)
        if (*p < '0' || *p > '9') return 0;
    return 1;
}

static int es_temporal(const char *s) {
    if (!s || s[0] != 't') return 0;
    return es_entero(s + 1);
}

/* Indice de la variable segun su nombre unico (v_0, v_1, ...). */
static int var_indice(const char *unico) {
    int i = 0;
    for (TacSim *s = tac_simbolos(); s; s = s->sig, i++)
        if (strcmp(s->unico, unico) == 0) return i;
    return -1;
}

/* Valor entero de un literal (los 'dec' se truncan a entero). */
static int valor_literal(const char *s) {
    return atoi(s);   /* atoi se detiene en el '.' de los dec */
}

/* ¿El programa usa alguna cuadrupla con esta operacion? Sirve para
   emitir solo los runtimes de juego que hagan falta (teclado, espera,
   aleatorio) y no engordar los programas que no los usan. */
static int tac_usa(const char *op) {
    for (const Cuad *c = tac_cuadruplas(); c; c = c->sig)
        if (strcmp(c->op, op) == 0) return 1;
    return 0;
}

/* ── Carga de operandos en HL / DE ──────────────────────────── */
static void cargar(const char *reg, const char *op) {
    int vi;
    if (es_cadena(op)) {
        printf("        ld %s,str_%d\n", reg, str_indice(op));
    } else if ((vi = var_indice(op)) >= 0) {
        printf("        ld %s,(v_%d)\n", reg, vi);
    } else if (es_temporal(op)) {
        printf("        ld %s,(t_%s)\n", reg, op + 1);
    } else {
        printf("        ld %s,%d\n", reg, valor_literal(op) & 0xFFFF);
    }
}

static void guardar_hl(const char *dest) {
    int vi = var_indice(dest);
    if (vi >= 0)                  printf("        ld (v_%d),hl\n", vi);
    else if (es_temporal(dest))   printf("        ld (t_%s),hl\n", dest + 1);
}

/* HL = HL invertido como booleano 0/1 (para <=, >=, !=). */
static void invertir_bool(void) {
    printf("        ld a,l\n");
    printf("        xor 1\n");
    printf("        ld l,a\n");
    printf("        ld h,0\n");
}

/* ── Traduccion de una cuadrupla ────────────────────────────── */
static void traducir(const Cuad *c, int n) {
    char txt[512];
    tac_formatear(c, txt, sizeof(txt));
    const char *op = c->op;

    if (strcmp(op, "label") == 0) {
        printf("L_%s:                          ; [%d] %s\n", c->res + 1, n, txt);
        return;
    }
    printf("; [%d] %s\n", n, txt);

    if (strcmp(op, "=") == 0) {
        cargar("hl", c->arg1);
        guardar_hl(c->res);

    } else if (strcmp(op, "goto") == 0) {
        printf("        jp L_%s\n", c->res + 1);

    } else if (strcmp(op, "ifFalse") == 0) {
        cargar("hl", c->arg1);
        printf("        ld a,h\n");
        printf("        or l\n");
        printf("        jp z,L_%s\n", c->res + 1);

    } else if (strcmp(op, "imprimir") == 0) {
        if (c->arg2 && strcmp(c->arg2, "text") == 0) {
            cargar("hl", c->arg1);          /* direccion de la cadena */
            printf("        call __prstr\n");
        } else {
            cargar("hl", c->arg1);
            printf("        call __prnum\n");
        }
        printf("        call __prnl\n");

    } else if (strcmp(op, "retornar") == 0) {
        printf("        jp __fin\n");

    } else if (strcmp(op, "fin") == 0) {
        /* __fin viene inmediatamente despues del cuerpo */

    /* ── Primitivas de juego (Amstrad CPC) ─────────────────── */
    } else if (strcmp(op, "cls") == 0) {
        printf("        ld a,12             ; CHR$(12) = limpiar pantalla\n");
        printf("        call &BB5A\n");

    } else if (strcmp(op, "locate") == 0) {
        printf("        ld a,31             ; control 31 = posicionar cursor\n");
        printf("        call &BB5A\n");
        cargar("hl", c->arg1);              /* columna */
        printf("        ld a,l\n");
        printf("        call &BB5A\n");
        cargar("hl", c->arg2);              /* fila */
        printf("        ld a,l\n");
        printf("        call &BB5A\n");

    } else if (strcmp(op, "draw") == 0) {
        /* como imprimir pero SIN salto de linea (dibujo en pantalla) */
        if (c->arg2 && strcmp(c->arg2, "text") == 0) {
            cargar("hl", c->arg1);
            printf("        call __prstr\n");
        } else {
            cargar("hl", c->arg1);
            printf("        call __prnum\n");
        }

    } else if (strcmp(op, "putc") == 0) {
        cargar("hl", c->arg1);              /* codigo de caracter */
        printf("        ld a,l\n");
        printf("        call &BB5A\n");

    } else if (strcmp(op, "wait") == 0) {
        cargar("hl", c->arg1);              /* numero de cuadros */
        printf("        call __esperar\n");

    } else if (strcmp(op, "sprites") == 0) {
        printf("        call __defsprites\n");  /* define los caracteres 240..243 */

    } else if (strcmp(op, "tecla") == 0) {
        printf("        call __tecla\n");   /* HL = codigo de tecla (0 si nada) */
        guardar_hl(c->res);

    } else if (strcmp(op, "rand") == 0) {
        printf("        call __rand\n");    /* HL = aleatorio 0..65535 */
        cargar("de", c->arg1);              /* DE = limite n */
        printf("        call __umod\n");    /* HL = HL mod DE (sin signo) */
        guardar_hl(c->res);

    } else if (strcmp(op, "neg") == 0) {
        cargar("hl", c->arg1);
        printf("        call __neghl\n");
        guardar_hl(c->res);

    } else if (strcmp(op, "not") == 0) {
        cargar("hl", c->arg1);
        printf("        call __not\n");
        guardar_hl(c->res);

    } else {
        /* Operaciones binarias. '>' y '>=' se traducen con __lt
           intercambiando los operandos: a>b == b<a.             */
        int invertido = (strcmp(op, ">") == 0 || strcmp(op, "<=") == 0);
        cargar("hl", invertido ? c->arg2 : c->arg1);
        cargar("de", invertido ? c->arg1 : c->arg2);

        if      (strcmp(op, "+") == 0) printf("        add hl,de\n");
        else if (strcmp(op, "-") == 0) { printf("        or a\n"); printf("        sbc hl,de\n"); }
        else if (strcmp(op, "*") == 0) printf("        call __mul16\n");
        else if (strcmp(op, "/") == 0) printf("        call __div16\n");
        else if (strcmp(op, "%") == 0) printf("        call __mod16\n");
        else if (strcmp(op, "<") == 0 || strcmp(op, ">") == 0)
            printf("        call __lt\n");
        else if (strcmp(op, "<=") == 0 || strcmp(op, ">=") == 0) {
            printf("        call __lt\n");
            invertir_bool();
        }
        else if (strcmp(op, "==") == 0) printf("        call __eq\n");
        else if (strcmp(op, "!=") == 0) { printf("        call __eq\n"); invertir_bool(); }
        else if (strcmp(op, "&&") == 0) {
            printf("        ld a,l\n");
            printf("        and e\n");
            printf("        ld l,a\n");
            printf("        ld h,0\n");
        }
        else if (strcmp(op, "||") == 0) {
            printf("        ld a,l\n");
            printf("        or e\n");
            printf("        ld l,a\n");
            printf("        ld h,0\n");
        }
        guardar_hl(c->res);
    }
}

/* ── Cadenas: bytes CPC con transliteracion basica ────────────
   El fuente llega en UTF-8; el CPC usa un juego tipo ASCII, asi
   que los acentos se transliteran (a,e,i,...) y \n pasa a 13,10. */
static int bytes_cadena(const char *lit, unsigned char *out, int max) {
    int n = 0;
    const unsigned char *p = (const unsigned char*)lit + 1;   /* salta la comilla inicial */
    while (*p && n < max - 2) {
        unsigned char ch = *p;
        if (ch == '"' && !*(p + 1)) break;                    /* comilla final */
        if (ch == '\\' && *(p + 1)) {
            p++;
            switch (*p) {
                case 'n': out[n++] = 13; out[n++] = 10; break;
                case 't': out[n++] = 9;  break;
                default:  out[n++] = *p; break;
            }
            p++;
            continue;
        }
        if (ch == 0xC3 && *(p + 1)) {                          /* UTF-8: letras acentuadas */
            unsigned char b = *(p + 1);
            /* pares (segundo byte UTF-8, letra equivalente) */
            const char *tabla = "\xA1" "a" "\xA9" "e" "\xAD" "i" "\xB3" "o"
                                "\xBA" "u" "\xB1" "n" "\xBC" "u"
                                "\x81" "A" "\x89" "E" "\x8D" "I" "\x93" "O"
                                "\x9A" "U" "\x91" "N" "\x9C" "U";
            char eq = '?';
            for (int i = 0; tabla[i]; i += 2)
                if ((unsigned char)tabla[i] == b) { eq = tabla[i + 1]; break; }
            out[n++] = (unsigned char)eq;
            p += 2;
            continue;
        }
        if (ch == 0xC2 && *(p + 1)) {                          /* ¡ / ¿ */
            unsigned char b = *(p + 1);
            out[n++] = (b == 0xA1) ? '!' : (b == 0xBF) ? '?' : '?';
            p += 2;
            continue;
        }
        out[n++] = (ch >= 32 && ch < 127) ? ch : '?';
        p++;
    }
    out[n] = 0;
    return n;
}

/* ── Listado completo ───────────────────────────────────────── */
static void generar_listado(void) {
    printf("; ============================================================\n");
    printf(";  Codigo Z80 generado por el Compilador Dummy (Grupo 5)\n");
    printf(";  Destino : Amstrad CPC (WinAPE) - se carga con RUN\"MAIN\n");
    printf(";  Firmware: &BC0E SCR_SET_MODE - &BB5A TXT_OUTPUT\n");
    printf("; ============================================================\n");
    printf("        org &4000\n");
    printf("\n");
    printf("inicio_prog:\n");
    printf("        ld a,1\n");
    printf("        call &BC0E          ; modo 1 (limpia la pantalla)\n");
    if (tac_usa("rand")) {
        printf("        call &BD0D          ; KL TIME PLEASE -> DE:HL (semilla)\n");
        printf("        ld (__seed),hl      ; siembra el generador aleatorio\n");
    }

    int n = 1;
    for (const Cuad *c = tac_cuadruplas(); c; c = c->sig, n++)
        traducir(c, n);

    /* Epilogo: no hay sistema operativo al que volver, asi que el
       programa se queda en un bucle infinito tras terminar (la
       salida ya quedo impresa en pantalla con imprimir()).        */
    printf("\n; --- fin del programa ---\n");
    printf("__fin:\n");
    printf("__stop:\n");
    printf("        jp __stop           ; el programa ya termino su trabajo\n");

    printf("\n; ============ rutinas de soporte (runtime) =================\n");
    printf("\n; --- imprime la cadena terminada en 0 apuntada por HL ---\n");
    printf("__prstr:\n");
    printf("        ld a,(hl)\n");
    printf("        or a\n");
    printf("        ret z\n");
    printf("        call &BB5A\n");
    printf("        inc hl\n");
    printf("        jr __prstr\n");

    printf("\n; --- salto de linea ---\n");
    printf("__prnl:\n");
    printf("        ld a,13\n");
    printf("        call &BB5A\n");
    printf("        ld a,10\n");
    printf("        call &BB5A\n");
    printf("        ret\n");

    printf("\n; --- imprime HL como entero con signo ---\n");
    printf("__prnum:\n");
    printf("        bit 7,h\n");
    printf("        jr z,__prnum1\n");
    printf("        ld a,45             ; '-'\n");
    printf("        call &BB5A\n");
    printf("        call __neghl\n");
    printf("__prnum1:\n");
    printf("        ld b,0              ; digitos apilados\n");
    printf("__prnum2:\n");
    printf("        ld de,10\n");
    printf("        call __udiv16       ; HL=cociente, DE=resto\n");
    printf("        ld a,e\n");
    printf("        add a,48            ; '0' + resto\n");
    printf("        push af\n");
    printf("        inc b\n");
    printf("        ld a,h\n");
    printf("        or l\n");
    printf("        jr nz,__prnum2\n");
    printf("__prnum3:\n");
    printf("        pop af\n");
    printf("        call &BB5A\n");
    printf("        djnz __prnum3\n");
    printf("        ret\n");

    printf("\n; --- HL = -HL ---\n");
    printf("__neghl:\n");
    printf("        ld a,l\n");
    printf("        cpl\n");
    printf("        ld l,a\n");
    printf("        ld a,h\n");
    printf("        cpl\n");
    printf("        ld h,a\n");
    printf("        inc hl\n");
    printf("        ret\n");

    printf("\n; --- DE = -DE ---\n");
    printf("__negde:\n");
    printf("        ex de,hl\n");
    printf("        call __neghl\n");
    printf("        ex de,hl\n");
    printf("        ret\n");

    printf("\n; --- HL = (HL == 0) ? 1 : 0 ---\n");
    printf("__not:\n");
    printf("        ld a,h\n");
    printf("        or l\n");
    printf("        ld hl,0\n");
    printf("        ret nz\n");
    printf("        inc l\n");
    printf("        ret\n");

    printf("\n; --- HL = (HL < DE) con signo ? 1 : 0 ---\n");
    printf("__lt:\n");
    printf("        ld a,h\n");
    printf("        xor d\n");
    printf("        jp m,__lt2          ; signos distintos\n");
    printf("        or a\n");
    printf("        sbc hl,de\n");
    printf("        jp c,__lt1\n");
    printf("        ld hl,0\n");
    printf("        ret\n");
    printf("__lt1:\n");
    printf("        ld hl,1\n");
    printf("        ret\n");
    printf("__lt2:\n");
    printf("        bit 7,h\n");
    printf("        jr nz,__lt1         ; HL negativo => menor\n");
    printf("        ld hl,0\n");
    printf("        ret\n");

    printf("\n; --- HL = (HL == DE) ? 1 : 0 ---\n");
    printf("__eq:\n");
    printf("        or a\n");
    printf("        sbc hl,de\n");
    printf("        ld hl,0\n");
    printf("        ret nz\n");
    printf("        inc l\n");
    printf("        ret\n");

    printf("\n; --- HL = HL * DE (mod 65536; vale con signo) ---\n");
    printf("__mul16:\n");
    printf("        push bc             ; preserva BC del llamador\n");
    printf("        ld c,h\n");
    printf("        ld a,l              ; CA = multiplicando\n");
    printf("        ld hl,0\n");
    printf("        ld b,16\n");
    printf("__mul1:\n");
    printf("        add hl,hl\n");
    printf("        sla a\n");
    printf("        rl c                ; CA <<= 1, sale el bit alto\n");
    printf("        jr nc,__mul2\n");
    printf("        add hl,de\n");
    printf("__mul2:\n");
    printf("        djnz __mul1\n");
    printf("        pop bc\n");
    printf("        ret\n");

    printf("\n; --- HL = HL / DE sin signo; DE = resto ---\n");
    printf("__udiv16:\n");
    printf("        push bc             ; preserva BC del llamador\n");
    printf("        ld a,h\n");
    printf("        ld c,l              ; AC = dividendo -> cociente\n");
    printf("        ld hl,0\n");
    printf("        ld b,16\n");
    printf("        or a                ; carry = 0\n");
    printf("__udiv1:\n");
    printf("        rl c\n");
    printf("        rla                 ; AC <<= 1 (entra bit de cociente)\n");
    printf("        adc hl,hl           ; resto = resto*2 + bit\n");
    printf("        or a\n");
    printf("        sbc hl,de\n");
    printf("        jr nc,__udiv2       ; cupo: bit de cociente = 1\n");
    printf("        add hl,de\n");
    printf("        or a                ; bit de cociente = 0\n");
    printf("        jr __udiv3\n");
    printf("__udiv2:\n");
    printf("        scf\n");
    printf("__udiv3:\n");
    printf("        djnz __udiv1\n");
    printf("        rl c\n");
    printf("        rla                 ; entra el ultimo bit\n");
    printf("        ld d,h\n");
    printf("        ld e,l              ; DE = resto\n");
    printf("        ld h,a\n");
    printf("        ld l,c              ; HL = cociente\n");
    printf("        pop bc\n");
    printf("        ret\n");

    printf("\n; --- HL = HL / DE con signo (trunca hacia cero) ---\n");
    printf("__div16:\n");
    printf("        ld a,h\n");
    printf("        xor d\n");
    printf("        push af             ; bit 7 = signo del cociente\n");
    printf("        bit 7,h\n");
    printf("        call nz,__neghl\n");
    printf("        bit 7,d\n");
    printf("        call nz,__negde\n");
    printf("        call __udiv16\n");
    printf("        pop af\n");
    printf("        and 128\n");
    printf("        ret z\n");
    printf("        jp __neghl\n");

    printf("\n; --- HL = HL %% DE con signo (signo del dividendo) ---\n");
    printf("__mod16:\n");
    printf("        ld a,h\n");
    printf("        push af             ; bit 7 = signo del dividendo\n");
    printf("        bit 7,h\n");
    printf("        call nz,__neghl\n");
    printf("        bit 7,d\n");
    printf("        call nz,__negde\n");
    printf("        call __udiv16\n");
    printf("        ex de,hl            ; HL = resto\n");
    printf("        pop af\n");
    printf("        and 128\n");
    printf("        ret z\n");
    printf("        jp __neghl\n");

    /* ── Runtimes de juego (solo si el programa los usa) ──────── */
    if (tac_usa("tecla")) {
        printf("\n; --- HL = codigo de la tecla pulsada (0 si ninguna) ---\n");
        printf("__tecla:\n");
        printf("        call &BB09          ; KM READ CHAR (no bloqueante)\n");
        printf("        jr c,__tecla_si\n");
        printf("        ld hl,0\n");
        printf("        ret\n");
        printf("__tecla_si:\n");
        printf("        ld l,a\n");
        printf("        ld h,0\n");
        printf("        ret\n");
    }

    if (tac_usa("wait")) {
        printf("\n; --- espera HL cuadros de video (sincroniza el juego) ---\n");
        printf("__esperar:\n");
        printf("        ld a,h\n");
        printf("        or l\n");
        printf("        ret z\n");
        printf("__esperar1:\n");
        printf("        push hl\n");
        printf("        call &BD19          ; MC WAIT FLYBACK (un cuadro)\n");
        printf("        pop hl\n");
        printf("        dec hl\n");
        printf("        ld a,h\n");
        printf("        or l\n");
        printf("        jr nz,__esperar1\n");
        printf("        ret\n");
    }

    if (tac_usa("sprites")) {
        printf("\n; --- define los caracteres de usuario 240..243 (CPC) ---\n");
        printf("; reserva 240..255 con &BBAB TXT SET M TABLE (DE=primer car,\n");
        printf("; HL=tabla de 8 bytes/car) y luego &BBA8 TXT SET MATRIX por sprite\n");
        printf("__defsprites:\n");
        printf("        ld de,240\n");
        printf("        ld hl,__mtable\n");
        printf("        call &BBAB          ; hace 240..255 definibles por usuario\n");
        printf("        ld a,240\n");
        printf("        ld hl,__spr_claude\n");
        printf("        call &BBA8          ; 240 = muneco de Claude (nave)\n");
        printf("        ld a,241\n");
        printf("        ld hl,__spr_ast1\n");
        printf("        call &BBA8          ; 241 = asteroide pequeno\n");
        printf("        ld a,242\n");
        printf("        ld hl,__spr_ast2\n");
        printf("        call &BBA8          ; 242 = asteroide mediano\n");
        printf("        ld a,243\n");
        printf("        ld hl,__spr_ast3\n");
        printf("        call &BBA8          ; 243 = asteroide grande\n");
        printf("        ret\n");
    }

    if (tac_usa("rand")) {
        printf("\n; --- HL = siguiente pseudoaleatorio (LCG de 16 bits) ---\n");
        printf("__rand:\n");
        printf("        ld hl,(__seed)\n");
        printf("        ld de,25173\n");
        printf("        call __mul16        ; HL = semilla * 25173\n");
        printf("        ld de,13849\n");
        printf("        add hl,de           ; + 13849  (mod 65536)\n");
        printf("        ld (__seed),hl\n");
        printf("        ret\n");
        printf("\n; --- HL = HL mod DE (sin signo) ---\n");
        printf("__umod:\n");
        printf("        call __udiv16       ; HL=cociente, DE=resto\n");
        printf("        ex de,hl            ; HL = resto\n");
        printf("        ret\n");
    }

    /* ── Datos: variables, temporales y cadenas ── */
    printf("\n; ==================== datos ================================\n");
    int i = 0;
    for (TacSim *s = tac_simbolos(); s; s = s->sig, i++)
        printf("v_%d:    defw 0             ; variable '%s' (%s)\n", i, s->unico, s->tipo);
    for (int t = 1; t <= tac_num_temporales(); t++)
        printf("t_%d:    defw 0             ; temporal t%d\n", t, t);

    if (tac_usa("rand"))
        printf("__seed: defw 7              ; semilla del generador aleatorio\n");

    if (tac_usa("sprites")) {
        printf("; --- tabla de matrices de usuario (240..255 -> 16*8 bytes) ---\n");
        printf("__mtable: defs 128\n");
        printf("; --- matrices 8x8 de los sprites (bit 7 = pixel izquierdo) ---\n");
        printf("__spr_claude: defb &3C,&42,&5A,&42,&3C,&18,&3C,&24  ; muneco de Claude\n");
        printf("__spr_ast1:   defb &00,&00,&18,&3C,&3C,&18,&00,&00  ; asteroide pequeno\n");
        printf("__spr_ast2:   defb &00,&3C,&7E,&7E,&7E,&7E,&3C,&00  ; asteroide mediano\n");
        printf("__spr_ast3:   defb &3C,&7E,&FF,&FF,&FF,&FF,&7E,&3C  ; asteroide grande\n");
    }

    for (int k = 0; k < n_strs; k++) {
        unsigned char buf[512];
        int len = bytes_cadena(strs[k], buf, sizeof(buf));
        printf("str_%d:  defb ", k);
        for (int j = 0; j < len; j++) printf("%d,", buf[j]);
        printf("0                ; %s\n", strs[k]);
    }
}

/* La primera pasada registra las cadenas en el pool para que los
   'ld hl,str_N' del cuerpo coincidan con las defb del final: como
   str_indice() se llama al emitir cada carga, basta una pasada.  */

void imprimir_z80(void) {
    liberar_strs();
    printf("\n==================================================\n");
    printf("CODIGO ENSAMBLADOR Z80 (AMSTRAD CPC)\n");
    printf("==================================================\n\n");
    generar_listado();
    printf("\n==================================================\n");
    printf("FIN DEL CODIGO Z80\n");
    printf("==================================================\n");
    liberar_strs();
}

void emitir_z80_gui(void) {
    liberar_strs();
    generar_listado();
    liberar_strs();
}
