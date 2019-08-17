# include <iostream>
# include "Filler.h"

int main(int argc, char **argv)
{
    Filler filler("/home/hoarfrost/Documents/Cell/CellFilling/route.txt", 1);
    
    filler.Start();
    
    std::cout << "Found " << filler.combinationsCount << " combinations." << std::endl;
    
	return 0;
}
