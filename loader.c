#include <stdio.h>
#include <elf.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

extern int startup(int argc, char **argv, void (*start)());

// task0
//  void print_phdr(Elf32_Phdr *phdr, int i) {
//      printf("Program header number %d at address %p\n", i, phdr);
//  }

// task 1
void print_phdr(Elf32_Phdr *phdr, int i)
{
    const char *types[] = {"NULL", "LOAD", "DYNAMIC", "INTERP", "NOTE", "SHLIB", "PHDR", "TLS", "NUM", "LOOS", "GNU_EH_FRAME", "GNU_STACK", "GNU_RELRO", "LOSUNW", "SUNWBSS", "SUNWSTACK", "HISUNW", "HIOS", "LOPROC", "HIPROC"};
    int prot = 0;
    if (phdr->p_flags & PF_R)
        prot |= PROT_READ;
    if (phdr->p_flags & PF_W)
        prot |= PROT_WRITE;
    if (phdr->p_flags & PF_X)
        prot |= PROT_EXEC;
    printf("Type: %s, Offset: 0x%06x, VirtAddr: 0x%08x, PhysAddr: 0x%08x, FileSiz: 0x%05x, MemSiz: 0x%05x, Flags: %c%c%c, Align: %d, Protection flags: %d, Mapping flags: %d\n",
           types[phdr->p_type], phdr->p_offset, phdr->p_vaddr, phdr->p_paddr, phdr->p_filesz, phdr->p_memsz,
           (phdr->p_flags & PF_R ? 'R' : ' '),
           (phdr->p_flags & PF_W ? 'W' : ' '),
           (phdr->p_flags & PF_X ? 'X' : ' '),
           phdr->p_align, prot, MAP_PRIVATE);
}

int foreach_phdr(void *map_start, void (*func)(Elf32_Phdr *, int), int arg)
{
    Elf32_Ehdr *header = (Elf32_Ehdr *)map_start;
    Elf32_Phdr *phdr = (Elf32_Phdr *)(map_start + header->e_phoff);
    for (int i = 0; i < header->e_phnum; i++)
    {
        if (phdr[i].p_type == PT_LOAD) // Only load LOAD segments
        {
            func(&phdr[i], arg);
        }
    }
    return 0;
}

void load_phdr(Elf32_Phdr *phdr, int fd)
{
    if (phdr->p_type == PT_LOAD) {
        void* segment_address = mmap(
            (void*)(phdr->p_vaddr & 0xfffff000),           
            phdr->p_memsz + (phdr->p_vaddr & 0xfff),
            (phdr->p_flags & PF_X ? PROT_EXEC : 0) |
            (phdr->p_flags & PF_W ? PROT_WRITE : 0) |
            (phdr->p_flags & PF_R ? PROT_READ : 0),
            MAP_PRIVATE | MAP_FIXED,  
            fd,                 
            (phdr->p_offset & 0xfffff000)
        );

        if (segment_address == MAP_FAILED) {
            printf("error in mapping to address %p\n", (void*)phdr->p_vaddr);
        } else {
            print_phdr(phdr, 0);
        }
    }
}

int main(int argc, char **argv)
{
    char* program = argv[1];
    int fd = open(program, O_RDONLY);
    struct stat st;
    fstat(fd, &st);
    size_t size = st.st_size;
    void *map_start = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
    // foreach_phdr(map_start, print_phdr, 0);
    foreach_phdr(map_start, load_phdr, fd);
    startup(argc - 1, argv + 1, (void *)(((Elf32_Ehdr *)map_start)->e_entry));
    munmap(map_start, size);
    close(fd);
    return 0;
}
