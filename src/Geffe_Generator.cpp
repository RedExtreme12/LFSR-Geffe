#include <iostream>
#include <vector>
#include "lfsr.h"

lfsr_data_t init_state_1 = 0x4EAAF4;
lfsr_data_t init_state_2 = 0x10FEFB40;
lfsr_data_t init_state_3 = 0x4CFB375E;

lfsr_data_t polynom_1 = 0x40003B;
lfsr_data_t polynom_2 = 0x1000009D;
lfsr_data_t polynom_3 = 0x40000040;

void GLFSR_init(lfsr_t* glfsr, lfsr_data_t polynom, lfsr_data_t seed_value, lfsr_data_t degree) 
{
    lfsr_data_t seed_mask;
    unsigned int shift = 8 * sizeof(lfsr_data_t) - 1;
    glfsr->seed = seed_value;
    glfsr->data = seed_value;
    glfsr->degree = degree;

    lfsr_data_t poly = (polynom << degree);

    glfsr->polynomial = 0;
    while (poly != 0)
    {
        glfsr->polynomial <<= 1;
        glfsr->polynomial ^= (poly & 1);
        poly >>= 1;
    }

    seed_mask = 1; // самый большой бит, степень полинома.
    seed_mask <<= shift; 
        
    while (shift--) {
        if (polynom & seed_mask) {
            glfsr->mask = seed_mask;
            break;
        }
        seed_mask >>= 1;
    }
    return;

}

bool GLFSR_next(lfsr_t* glfsr) {
    if (glfsr->data & 1) 
    {
        glfsr->data ^= glfsr->polynomial;
        glfsr->data >>= 1;
        glfsr->data |= glfsr->mask;
        return 1;
    }
    else
    {
        glfsr->data >>= 1;
        return 0;
    }
}

void GLFSR_sequence_fill(lfsr_t* glfsr, std::vector <bool> &out, const size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        out[i] = GLFSR_next(glfsr);
    }
}

void geffe(std::vector <bool>& seq1, std::vector <bool>& seq2, std::vector <bool>& seq3, std::vector <bool>& out_seq, const size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        out_seq[i] = ((seq1[i] & seq2[i]) | ((~seq1[i]) & seq3[i]));
    }
}

void print_vector(std::vector <bool> sequence)
{
    for (size_t i = 0; i < sequence.size(); i++)
    {
        std::cout << sequence[i];
    }

    std::cout << std::endl;
}

void write_data(std::vector<bool> text, int size, FILE* ptr)
{
    for (int i = 0; i < size; i += 8)
    {
        short a = text[i];
        for (int j = 1; j < 8; j++)
        {
            a <<= 1;
            a |= text[i + j];
        }
        fwrite(&a, sizeof(bool), 1, ptr);
    }
}

int main()
{
    FILE* file_output;
    fopen_s(&file_output, "result.bin", "wb");

    int size;
    std::cout << "Enter size of sequence: " << std::endl;
    std::cin >> size;

    lfsr_t lfsr_1;
    lfsr_t lfsr_2;
    lfsr_t lfsr_3;

    GLFSR_init(&lfsr_1, polynom_1, init_state_1, 23);
    GLFSR_init(&lfsr_2, polynom_2, init_state_2, 29);
    GLFSR_init(&lfsr_3, polynom_3, init_state_3, 31);

    std::vector <bool> seq1(size);
    std::vector <bool> seq2(size);
    std::vector <bool> seq3(size);
    std::vector <bool> out_seq(size);

    GLFSR_sequence_fill(&lfsr_1, seq1, size);
    GLFSR_sequence_fill(&lfsr_2, seq2, size);
    GLFSR_sequence_fill(&lfsr_3, seq3, size);

    geffe(seq1, seq2, seq3, out_seq, size);

    print_vector(out_seq);

    if (file_output)
    {
        write_data(out_seq, size, file_output);

        fclose(file_output);
    }

    return 0;
}