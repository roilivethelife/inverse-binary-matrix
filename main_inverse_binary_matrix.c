/*
 * Copyright (C) 2021 Roi Rodríguez Huertas
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>


void printMatrix(uint8_t *matrix, int size);
void printTwoMatrix(uint8_t *matrix, uint8_t *matrix2, int size);

void setIdentityMatrix(uint8_t *matrix, int size);
void xorRows(uint8_t *matrix, int size, int filaResultado, int filaOperacion);



#define MATRIX_SIZE 32
uint32_t diffusion[MATRIX_SIZE] = {
    0xf26cb481, 0x16a5dc92, 0x3c5ba924, 0x79b65248, 0x2fc64b18, 0x615acd29, 0xc3b59a42, 0x976b2584,
    0x6cf281b4, 0xa51692dc, 0x5b3c24a9, 0xb6794852, 0xc62f184b, 0x5a6129cd, 0xb5c3429a, 0x6b978425,
    0xb481f26c, 0xdc9216a5, 0xa9243c5b, 0x524879b6, 0x4b182fc6, 0xcd29615a, 0x9a42c3b5, 0x2584976b,
    0x81b46cf2, 0x92dca516, 0x24a95b3c, 0x4852b679, 0x184bc62f, 0x29cd5a61, 0x429ab5c3, 0x84256b97
};



//Two matrix defined as arrays
uint8_t inputMatrix[MATRIX_SIZE * MATRIX_SIZE];
uint8_t inverseMatrix[MATRIX_SIZE * MATRIX_SIZE];

/**
 * Loads difussion matrix into inputMatrix
 */
void loadMatrix(){
    for (int fila = 0; fila < MATRIX_SIZE; fila++){
        for(int col = 0; col < MATRIX_SIZE; col++){
            inputMatrix[fila*MATRIX_SIZE+col]=((diffusion[fila] >> col)&1);
        }
    }
}

void matriz2arrayPrint(){
    printf("uint32_t array[MATRIX_SIZE] = {\n");
    for (int fila = 0; fila < MATRIX_SIZE; fila++){
        uint32_t numeroFila=0;
        for(int col = 0; col < MATRIX_SIZE; col++){
            numeroFila |=  (inverseMatrix[fila*MATRIX_SIZE+col]) << col;
        }
        printf("0x%08X, ",numeroFila);
        if( fila%8 == 7 ) printf("\n");
    }
}

void calculateInverseByGaussian() {
    
    for (int row = 0; row < MATRIX_SIZE; row++) {
        printf("1-Processing Row=%d\n",row);
        
        //check if matrix[row][row] already==1
        if (*(inputMatrix + row * MATRIX_SIZE + row) == 0) {
            printf("M[%d][%d]==0\n",row,row);
            
            //search for another row with 1 in column=row
            for (int rowSearch = row + 1; rowSearch < MATRIX_SIZE; rowSearch++) {
                
                //Found, now XOR(found->row) to put 1 in [row][column]
                if (*(inputMatrix + rowSearch * MATRIX_SIZE + row) == 1) {
                    
                    printf("XOR F(%d)->F(%d)\n", rowSearch, row);
                    xorRows(inputMatrix, MATRIX_SIZE, row, rowSearch);
                    xorRows(inverseMatrix, MATRIX_SIZE, row, rowSearch);
                    
                    break;
                }
                
            }
        }
        
        
        //do xor to the rest of rows that have matrix[N][row]==1
        for (int rowSearch = row + 1; rowSearch < MATRIX_SIZE; rowSearch++) {
            if (*(inputMatrix + rowSearch * MATRIX_SIZE + row) == 1) {
                printf("XOR F(%d)->F(%d)\n", row, rowSearch);
                xorRows(inputMatrix, MATRIX_SIZE, rowSearch, row);
                xorRows(inverseMatrix, MATRIX_SIZE, rowSearch, row);
                //printTwoMatrix(inputMatrix, inverseMatrix, MATRIX_SIZE);
            }
        }
    }
    
    // THE LOWER LEFT SIDE OF THE MATRIX SHOULD BE ALL 0'S
    // AND THE DIAGONAL ALL 1'S
    // NOW WE CLEAN THE REST OF THE MATRIX EASY STARTING BOTTOM TO THE TOP
    
    
    for (int row = MATRIX_SIZE-1; row>=0; row--) {
        printf("2-PROCESING ROW=%d\n",row);
        //check matrix[fila][fila]==1
        if (*(inputMatrix + row * MATRIX_SIZE + row) == 0) {
            printf("NOT INVERTIBLE!!!!\n");
            return;
        }
        
        
        //xor rows that matrix[N][row]==1
        for (int rowSearch = row-1; rowSearch >=0; rowSearch--) {
            if (*(inputMatrix + rowSearch * MATRIX_SIZE + row) == 1) {
                printf("XOR F(%d)->F(%d)\n", row, rowSearch);
                xorRows(inputMatrix, MATRIX_SIZE, rowSearch, row);
                xorRows(inverseMatrix, MATRIX_SIZE, rowSearch, row);
                //printTwoMatrix(inputMatrix, inverseMatrix, MATRIX_SIZE);
            }
        }        
    }
    
    //DONE!
    printTwoMatrix(inputMatrix, inverseMatrix, MATRIX_SIZE);
    //matriz2arrayPrint();
}


/*
 * 
 */
int main(int argc, char** argv) {

    loadMatrix();//inputMatrix = difussion
    setIdentityMatrix(inverseMatrix, MATRIX_SIZE); //inverseMatrix = Identity
    printf("INPUT MATRIX | IDENTITY MATRIX:\n");
    printTwoMatrix(inputMatrix, inverseMatrix, MATRIX_SIZE);
    printf("-------------\n");

    /*
     MANUAL MODE
    int modo = 1;
    printf("Selecione modo-> 0=manual 1->auto\n->");
    scanf("%d", &modo);
    if (modo == 0) {
        modoManual();
    } else {*/
        calculateInverseByGaussian();
    //}



    return (EXIT_SUCCESS);
}

void printMatrix(uint8_t *matrix, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            printf("%hhu ", (uint8_t) *(matrix + i * size + j));
        }
        printf("\n");
    }
}

void printTwoMatrix(uint8_t *matrix, uint8_t *matrix2, int size) {
    for (int i = 0; i < size; i++) {
        printf("(%02d)  ", i);
        for (int j = 0; j < size; j++) {
            printf("%hhu ", (uint8_t) *(matrix + i * size + j));
        }
        printf(" | ");
        for (int j = 0; j < size; j++) {
            printf("%hhu ", (uint8_t) *(matrix2 + i * size + j));
        }
        printf("\n");
    }
}

/**
 * Sets identity matrix 
 * @param matrix
 * @param size
 */
void setIdentityMatrix(uint8_t *matrix, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (i == j) *(matrix + i * size + j) = 1;
            else *(matrix + i * size + j) = 0;
        }
    }
}


/**
 * filaResult = filaResult XOR filaOperation
 * @param matrix
 * @param size 
 * @param filaResult
 * @param filaOperation
 */
void xorRows(uint8_t *matrix, int size, int filaResult, int filaOperation) {
    for (int c = 0; c < size; c++) {
        *(matrix + filaResult * size + c) = (*(matrix + filaResult * size + c))^(*(matrix + filaOperation * size + c));
    }
}


/*MANUAL MODE

void modoManualXOR(char c_in) {
    //Leer 1 numero hasta coma, leeer 2 numero hasta\n
    char num1[8];
    char num2[8];
    int i = 0;
    char c = c_in;
    do {
        if (isdigit(c)) {
            num1[i] = c;
        } else {
            if (c == ' ')
                break;
            else {
                printf("Error parsing number\n->");
                return;
            }
        }
        i++;
        if (i > 6) {
            printf("Error parsing number\n->");
            return;
        }
        c = getchar();
    } while (1);
    num1[i] = '\0';

    i = 0;
    while (1) {
        char c = getchar();
        if (isdigit(c)) {
            num2[i] = c;
        } else {
            if (c == '\n')
                break;
            else {
                printf("Error parsing number\n->");
                return;
            }
        }
        i++;
        if (i > 6) {
            printf("Error parsing number\n->");
            return;
        }
    }
    num2[i] = '\0';

    int fila1 = atoi(num1);
    int fila2 = atoi(num2);
    printf("XOR(%d->%d)\n", fila1, fila2);
    xorFilas(inputMatrix, MATRIX_SIZE, fila2, fila1);
    xorFilas(inverseMatrix, MATRIX_SIZE, fila2, fila1);
    printTwoMatrix(inputMatrix, inverseMatrix, MATRIX_SIZE);
    printf("\n->");


}

void modoManual() {
    printf("Modo manual\n");
    printf("   p   = print\n"
            "   a b = xor a->b\n"
            "   e   = exit\n");
    printf("->");
    while (1) {
        char c = getchar();
        if (isdigit(c)) {
            modoManualXOR(c);
        } else {
            switch (c) {
                case 'p':
                case 'P':
                    printTwoMatrix(inputMatrix, inverseMatrix, MATRIX_SIZE);
                    break;
                case 'e':
                case 'E':
                    return;
                case '\n':
                    printf("->");
                    break;
                default:
                    break;
            }
        }
    }
}
*/
