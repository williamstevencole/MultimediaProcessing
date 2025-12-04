#include <fstream>
#include <string>
#include <iostream>

void writeFile(){
    std::ofstream archivo("estudiantes.txt");
    if(!archivo) std::cerr << "could not open file";

    int numestudiantes;
    std::cout << "Ingrese cuantos estudiantes desea ingresar: ";
    std::cin >> numestudiantes;

    int counter =0 ;

    while(counter < numestudiantes){
        std::string nombre;
        int edad;

        std::cout << "ingrese el nombre del " << counter << " estudiante" << std::endl;
        std::cin >> nombre;

        std::cout << "ingrese la edad de " << nombre << std::endl;
        std::cin >> edad;

        archivo << nombre << " " << edad << std::endl;

        counter++;
    }

    std::cout << "se guardaron correctamente los estudiantes :D" << std::endl;

    archivo.close();
}

void readFile(){
    std::ifstream archivo("estudiantes.txt");
    if(!archivo.is_open()) std::cerr << "no se pudo ingresar al archivo";

    std::string nombre;
    int edad;

    while(archivo >> nombre >> edad){
        std::cout << "Nombre: " << nombre << " Edad: " << edad << std::endl;
    }

}

int main(){
    writeFile();
    readFile();

    return 0;
}