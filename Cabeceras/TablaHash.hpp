//
// Created by Laser on 03/05/2025.
//

#ifndef TABLAHASH_HPP
#define TABLAHASH_HPP

template<typename Clave, typename Valor>
struct TablaHashNodo {
    Clave clave;
    Valor valor;
    TablaHashNodo *siguiente = nullptr;

    TablaHashNodo(const Clave &k, const Valor &v): clave(k), valor(v) {
    }
};

template<typename Clave, typename Valor, typename Hash>
class TablaHash {
public:
    explicit TablaHash(size_t capacidad_Inicial = 101);

    ~TablaHash();

    bool Insertar(const Clave &clave, const Valor &valor);

    bool Buscar(const Clave &clave, Valor &valor_encontrado) const;

    bool Eliminar(const Clave &clave);

    size_t ObtenerNumElementos() const;

    size_t ObtenerCapacidad() const;

private:
    TablaHashNodo **cubos;
    size_t tam_Tabla;
    size_t num_Elementos;
    Hash funcionHash;

    size_t obtenerIndice(const Clave &clave) const;
};
#endif //TABLAHASH_HPP
