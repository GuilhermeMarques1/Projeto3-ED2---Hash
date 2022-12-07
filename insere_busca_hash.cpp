#include<stdio.h>
#include<curses.h>

int main() {
    FILE *fd;
    
    //////////////////////////////
    typedef struct veiculo {
        char cod_cli[3];
        char cod_vei[3];
        char client[50];
        char veiculo[50];
        char dias[4];
    } veic_t;
	
	veic_t regs_locs_vei[8] = {{"11", "34", "Cliente-1", "Veiculo-11", "2"},
		                		{"22", "34", "Cliente-2", "Veiculo-11", "8"},
		                		{"33", "74", "Cliente-3", "Veiculo-33", "1"},
		                		{"44", "61", "Cliente-4", "Veiculo-44", "11"},
		                		{"11", "61", "Cliente-1", "Veiculo-44", "2"},
		                		{"11", "74", "Cliente-1", "Veiculo-66", "3"},
		                		{"77", "87", "Cliente-7", "Veiculo-22", "2"},
		                		{"88", "14", "Cliente-8", "Veiculo-88", "2"}};
       
    fd = fopen("insere.bin", "w+b");
    fwrite(regs_locs_vei, sizeof(regs_locs_vei), 1, fd);
    fclose(fd);
    
    //////////////////////////////
    typedef struct remove {
        char cod_cli[3];
        char cod_vei[3];
    } reg_id_t; 
	
	reg_id_t regs_id_list[5] = {{"33", "74"},
                  				{"11", "61"},
                  				{"77", "87"},
                  				{"44", "61"},
                  				{"99", "00"}};
       
    fd = fopen("busca.bin", "w+b");
    fwrite(regs_id_list, sizeof(regs_id_list), 1, fd);
    fclose(fd);
    
    
}

