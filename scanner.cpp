#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <stack>
#include <map>
#include <cctype>
using namespace std;

void comentarios(string& linea);
bool salto_de_linea(string& linea);
void printfile(vector<string> datos_);
//string file_cont = "";

vector<string> keywords = { "and", "as", "assert", "async", "await", "bool", "break", "class","continue", "def", "del", "elif", "else", "except", "False", "finally","float", "for", "from", "global", "if", "import", "in", "int", "is","lambda", "None", "nonlocal", "not", "or", "pass", "raise", "return","str", "True", "try", "while", "with", "yield"};
vector<string> operadores = { "+", "-", "*", "/","//", "%", "<", ">", "<=", ">=", "==", "!=", "=", ",", ":", ".", "->" };
vector<string> delimitadores = { "(", ")", "[", "]" };

class Token
{
public:
    string valor;
    string tipo;
    int fila;
    int columna;
    string error;

    Token(string valor_, string tipo_, int fila_, int columna_, string error_ = "")
    {
        valor = valor_;
        tipo = tipo_;
        fila = fila_;
        columna = columna_;
    }
};


class Scanner
{
    string filename;
    string file_cont;
    vector<Token> tokens;
    vector<Token> error;
    int posicion;
public:
    Scanner(string filename_)
    {
        filename = filename_;
        file_cont = "";
        posicion = 0;
    }

    void readF(); // leo el archivo linea por linea y lo almaceno en filecont
    void printfile(vector<string> datos_); // imprimo el archivo
    void comentarios(string& linea); // elimina comentarios
    bool salto_de_linea(string& linea);
    void scanTokens();
    void scanKeywords();
    char getChar();
    char peekChar();
};

char Scanner::getChar()
{
    if (posicion >= file_cont.size()) {
        return EOF;
    }
    char c = file_cont[posicion];
    posicion++;
    return c;
}

char Scanner::peekChar()
{
    if (posicion < file_cont.size()) {
        return file_cont[posicion];
    }
    else {
        return '\0';
    }
}

void Scanner::readF() {

    // String datos;
    
    ifstream archivo(filename.c_str());
    vector<string> datos_;
    string linea = "";
    int t = 0;

    // Revisamos si el archivo existe
    if (!archivo.is_open())
    {
        cout << "Error ! Could not open the file - '"<< filename << "'" << endl;
        exit(EXIT_FAILURE);
        return;
    }

    // Obtener linea de archivo, y almacenar contenido en "linea"
    while (getline(archivo, linea)) {
        if (linea.find("#") == 0) continue;
        comentarios(linea);
        if (salto_de_linea(linea))
        {
           // linea += " endl ";
        }//linea += " ENDL "; // verifica si hay un salto de linea
        datos_.push_back(linea);        //cout << linea << endl;
        //cout << linea<<" ->"<<t;
        //t++;
    } //cout<<"tam array: "<<t<<endl;
    archivo.close();

    // Juntamos las lineas en file_cont
    for (int i = 0; i < datos_.size(); i++) {
        file_cont += datos_[i];
        if (i < datos_.size() - 1)file_cont += "\n";
    }
    printfile(datos_);

}

void Scanner::scanKeywords()
{
    int fila = 1;
    int columna = 1;
    string token_value = "";
    int identacion = 0; // va a obtener la cantidad de espacios
    stack<int> pila_identacion;

    while (posicion < file_cont.size())
    {
        char c = getChar();

        // Identificar y agregar keyword
        if (isalpha(c) || c == '_')
        {
            token_value += c;
            char next_c = peekChar();
            while ((isalpha(next_c) || isdigit(next_c) || next_c == '_') && next_c != EOF)
            {
                token_value += getChar();
                next_c = peekChar();
            }

            if (find(keywords.begin(), keywords.end(), token_value) != keywords.end()) // buscamos el valor del token en las keywords
            {
                string token_keyword = token_value;
                for (auto& c : token_keyword) {
                    c = std::toupper(c);
                }
                tokens.push_back(Token(token_value, token_keyword, fila, columna));
            }
            else // si no es keyword, es identificador
            {
                string token_id = token_value;
                for (auto& c : token_id) {
                    c = std::toupper(c);
                }
                tokens.push_back(Token(token_value, token_id, fila, columna));
            }

            columna += token_value.size(); // Incrementar la columna por la longitud del token
            token_value = ""; // se resetea el valor del token, se busca el siguiente
        }

        // Identificar y agregar operador
        else if (find(operadores.begin(), operadores.end(), string(1, c)) != operadores.end())
        {
            string operador = string(1, c);
            char next_c = peekChar();
            if ((c == '+' && next_c == '+') || (c == '-' && next_c == '-') || (c == '-' && next_c == '>') || (c == '=' && next_c == '='))
            {
                operador += getChar();
                //columna++;
            }
            if (operador == "++") tokens.push_back(Token(operador, "DOUB_SUM", fila, columna));
            if (operador == "--") tokens.push_back(Token(operador, "DOUB_RES", fila, columna));
            if (operador == "->") tokens.push_back(Token(operador, "ARROW", fila, columna));
            if (operador == "==") tokens.push_back(Token(operador, "COMPARACION", fila, columna));
            if (operador == ">=") tokens.push_back(Token(operador, "MAYOR_EQ", fila, columna));
            if (operador == "<=") tokens.push_back(Token(operador, "MENOR_EQ", fila, columna));
            if (operador == "!=") tokens.push_back(Token(operador, "DIF", fila, columna));

            if (operador == "+") tokens.push_back(Token(operador, "SUM", fila, columna));
            if (operador == "-") tokens.push_back(Token(operador, "RES", fila, columna));
            if (operador == "*") tokens.push_back(Token(operador, "MULT", fila, columna));
            if (operador == "/") tokens.push_back(Token(operador, "SLASH", fila, columna));

            if (operador == "//") tokens.push_back(Token(operador, "DOUB_SLASH", fila, columna));
            if (operador == "%") tokens.push_back(Token(operador, "MOD", fila, columna));
            if (operador == ">") tokens.push_back(Token(operador, "MAYOR", fila, columna));
            if (operador == "<") tokens.push_back(Token(operador, "MENOR", fila, columna));
            if (operador == "=") tokens.push_back(Token(operador, "ASIGNACION", fila, columna));
            if (operador == ",") tokens.push_back(Token(operador, "COMA", fila, columna));
            if (operador == ":") tokens.push_back(Token(operador, "DOUB_POINT", fila, columna));
            if (operador == ".") tokens.push_back(Token(operador, "PUNTO", fila, columna));
            //tokens.push_back(Token(operador, "operator", fila, columna));
            columna+=operador.size(); // Incrementar la columna por un caracter
        }

        // Identificar y agregar delimitador
        else if (find(delimitadores.begin(), delimitadores.end(), string(1, c)) != delimitadores.end())
        {
            if (string(1,c) == "(") tokens.push_back(Token(string(1, c), "LPAREN", fila, columna));
            if (string(1,c) == ")") tokens.push_back(Token(string(1, c), "RPAREN", fila, columna));
            if (string(1,c) == "[") tokens.push_back(Token(string(1, c), "LCORCH", fila, columna));
            if (string(1,c) == "]") tokens.push_back(Token(string(1, c), "RCORCH", fila, columna));
            //tokens.push_back(Token(string(1, c), "delimiter", fila, columna));
            columna++; // Incrementar la columna por un caracter
        }

        // Identificar y agregar número
        else if (isdigit(c))
        {
            if (c == '0')
            {

                columna++;
                posicion++;
                continue;
            }
            token_value += c;
            char next_c = peekChar();
            while ((isdigit(next_c) || next_c == '.') && next_c != EOF)
            {
                token_value += getChar();
                next_c = peekChar();
            }
            if (token_value.find(".") != string::npos) {
                tokens.push_back(Token(token_value, "FLOTANTE", fila, columna));
            }
            else {
                long long int_value = stoll(token_value);
                if (int_value > INT_MAX) {
                    error.push_back(Token(token_value, "ERROR", fila, columna, "ENTERO SOBREPASA EL LIMITE"));
                }
                tokens.push_back(Token(token_value, "ENTERO", fila, columna));
            }

            //tokens.push_back(Token(token_value, "NUMERO", fila, columna));
            columna += token_value.size(); // Incrementar la columna por la longitud del token
            token_value = "";
        }

        // codigo para identificar strings dentro de " "
        else if (c == '"')
        {
            token_value += c;
            char next_c = peekChar();
            while (next_c != '"' && next_c != EOF)
            {
                token_value += getChar();
                next_c = peekChar();
            }
            token_value += getChar(); // Agregar la última comilla doble al valor del token
            tokens.push_back(Token(token_value, "STRING", fila, columna));
            columna += token_value.size(); // Incrementar la columna por la longitud del token
            token_value = ""; // se resetea el valor del token, se busca el siguiente
        }


        // Identificar y saltar espacios en blanco
        else if (isspace(c))
        {
            char next_c = peekChar();
            if (c == '\n')
            {
                if (columna != 1)
                {
                    tokens.push_back(Token("", "NEWLINE", fila, columna));
                    fila++;
                    columna = 1;
                }
 
                    while (next_c == ' ')
                    {
                        if (next_c == ' ') {
                            next_c = getChar();
                        }
                        identacion++; // aumenta el numero de espacios
                        //tokens.push_back(Token("ESPACIO", "ESPACIO", fila, columna));
                        columna++;
                        if (next_c != ' ') posicion--;
                    }
                    //columna--; // temporal, debe solucionarse, aumenta en un espacio más cuando solo hay espacios al inicio de una linea
                    // termina de contar los espacios
                    if (pila_identacion.empty()) // si la pila está vacia, se añade el numero de identacion
                    {
                        pila_identacion.push(identacion);
                        identacion = 0; // se resetea para volver a contar
                        tokens.push_back(Token("", "IDENTACION", fila, 1));
                    }
                    else // comparo el numero de espacios nuevo con el de la pila
                    {
                        if (identacion > pila_identacion.top()) // pila: 3 , espacios 5 hubo identacion
                        {
                            pila_identacion.pop(); // se saca y se mete el nuevo valor
                            pila_identacion.push(identacion);
                            tokens.push_back(Token("", "IDENTACION", fila, 1));
                        }
                        else if (identacion < pila_identacion.top())// si el nuevo valor es menor, hubo dedentacion
                        {
                            pila_identacion.pop(); // se saca y se mete el nuevo valor
                            pila_identacion.push(identacion);
                            tokens.push_back(Token("", "DEDENTACION", fila, 1));
                        }
                    }
                    identacion = 0; 
                
            }
            else
            {
                columna++;
            }
        }



        // Caracter no reconocido, agregar error
        else
        {
            error.push_back(Token(string(1, c), "ERROR", fila, columna, "Caracter no reconocido"));
            columna++; // Incrementar la columna por un caracter
        }
    }
}








void Scanner::scanTokens()
{
    scanKeywords();

    for (int i = 0; i < tokens.size(); i++) {
        //cout << "Token encontrado en la fila " << tokens[i].fila << ", columna " << tokens[i].columna << ": " << tokens[i].valor << " (" << tokens[i].tipo << ")" << endl;
        cout << "DEBUG SCAN - " << tokens[i].tipo << "  [" << tokens[i].valor << "] found at (" << tokens[i].fila << ":" << tokens[i].columna << ")" << endl;
    }

    cout << "INFO SCAN - Completed with " << error.size() << " errors " << endl;
    if (error.size())
    {
        for (int i = 0; i < error.size(); i++) {
            //cout << "Token encontrado en la fila " << tokens[i].fila << ", columna " << tokens[i].columna << ": " << tokens[i].valor << " (" << tokens[i].tipo << ")" << endl;
            cout << "INFO SCAN - " << error[i].tipo << "  [" << error[i].valor << "] found at (" << error[i].fila << ":" << error[i].columna << ")" << endl;
        }
    }
}

void Scanner::printfile(vector<string> datos_)
{
    //cout << file_cont<<endl;
    for (int i = 0; i < datos_.size(); i++)
    {
        if (datos_[i].find('\n')) {
            cout << endl;
        }
        cout << datos_[i];
    }cout << endl;
}

void Scanner::comentarios(string &linea)
{
    /*static const regex comentario_regex(R"(\#.*$)");
    linea = regex_replace(linea, comentario_regex, "");*/

    size_t indice_comentario = linea.find("#");

    // Si se encuentra un comentario, eliminar la parte de la línea que sigue
    if (indice_comentario != string::npos) {
        linea = linea.substr(0, indice_comentario);
    }
}

bool Scanner::salto_de_linea(string& linea) {
    if (linea.find('\n')) return true;
    return false;
    //return (linea.find('\n') != string::npos);
}




int main()
{
    string filename = "";
    filename = "codigo.txt";
    Scanner scan(filename);
    scan.readF();
    scan.scanTokens();
    //readF(filename);
    //cout << file_cont;
    return 0;
}
