// File handling for C++

#include <iostream>
# include <string>

//Precisamos usar uma biblioteca para trabalhar com arquivos: 
#include<fstream>

int main()
{
  fstream myFile;
  
  // Se arquivo não existe, será criado
  myFile.open("name_of_the_file.txt", ios::out);  // output from program to file (write mode)
  
  if( myFile.is_open() ){
    myFile << "Hello\n";
    myFile << "Second line\n";
    myFile.close();
  }
  
  // Para adicionar (append) ao final do arquivo
   myFile.open("name_of_the_file.txt", ios::app);  // app for append
  
  if( myFile.is_open() ){
    myFile << "Hello2\n";
    myFile.close();
  }
  
  // Para ler um arquivo
  myFile.open("name.txt", ios::in); // read mode
  if (myFile.is_open() ){
    string line;
    while ( getline(myFile,line) {
      cout << line << endl;
    }
   myFile.close();
  }
 
  
}
