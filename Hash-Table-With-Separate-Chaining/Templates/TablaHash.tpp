/**
 * \file TablaHash.tpp
 * Este archivo contiene las implementaciones de los métodos de la clase template `TablaHash`.
 * Este archivo debe ser incluido al final del archivo de cabecera `.hpp`.
 * \author Joan Antonio Lazaro Silva
 * \date 06/05/2025
 */

// Abre el namespace donde está definida la clase
namespace myhashtable {

// --- Implementación de HashEntry ---
    template <typename K, typename V>
    TablaHash<K, V>::HashEntry::HashEntry() : ocupado(false), eliminado(false) {}

    template <typename K, typename V>
    TablaHash<K, V>::HashEntry::HashEntry(const K& k, const V& v) : clave(k), valor(v), ocupado(true), eliminado(false) {}

// --- Implementación de Estadisticas ---
    template <typename K, typename V>
    TablaHash<K, V>::Estadisticas::Estadisticas() {
        reset();
    }

    template <typename K, typename V>
    void TablaHash<K, V>::Estadisticas::reset() {
        inserciones = 0;
        busquedas = 0;
        eliminaciones = 0;
        colisiones = 0;
        rehashes = 0;
    }

// --- Implementación de la Clase TablaHash ---

// Constructor
    template <typename K, typename V>
    TablaHash<K, V>::TablaHash(size_t capacidadInicial, float factorMax, float factorMin, MetodoColision metodoColision)
            : elementos(0), elementos_eliminados(0), debugMode(false) {

        // Asegurar capacidad inicial mínima y que sea prima (o buscar la siguiente)
        // Usamos el siguiente primo >= capacidadInicial y al menos CAPACIDAD_MINIMA
        capacidad = encontrarSiguientePrimo(std::max(capacidadInicial, CAPACIDAD_MINIMA));

        // Validar y configurar factores de carga. Lanza excepción si son inválidos.
        configurarFactorCarga(factorMax, factorMin);

        // Configurar método inicial y redimensionar estructuras.
        // cambiarMetodo se encarga de redimensionar e inicializar 'tabla' o 'listas'.
        MetodoColision metodoInicial = metodo; // Guarda el método actual (puede ser el por defecto antes de la asignación)
        metodo = metodoColision; // Asigna el nuevo método para que cambiarMetodo funcione correctamente
        if (debugMode) {
            std::cout << "[DEBUG] Constructor: Capacidad inicial ajustada a primo: " << capacidad << std::endl;
        }
        cambiarMetodo(metodoInicial); // Llama a cambiarMetodo con el método que queremos, forzará la reconstrucción
        // Si el método inicial era el mismo que metodoColision, cambiarMetodo no hará rehash,
        // solo ajustará las estructuras según 'metodo' actual y capacidad.
        // Una alternativa sería llamar a cambiarMetodo(metodoColision) y que este maneje la inicialización inicial.
        // Simplificando: Inicializamos la estructura según el método elegido, sin reinsertar porque la tabla está vacía.
        if (metodo != ENCADENAMIENTO) {
            tabla.resize(capacidad); // Inicializa vector de HashEntry
        } else {
            listas.resize(capacidad); // Inicializa vector de listas
        }
        metodo = metodoColision; // Asegurarse que el método final sea el elegido

        stats.reset(); // Reiniciar estadísticas al inicio
        if (debugMode) {
            std::cout << "[DEBUG] Constructor: TablaHash inicializada con metodo " << nombreMetodo()
                      << ", capacidad " << capacidad << ", factores [" << factorCargaMin << ", " << factorCargaMax << "]\n";
        }
    }

// --- Funciones Hash (Implementación de ejemplo usando std::hash) ---
    template <typename K, typename V>
    size_t TablaHash<K, V>::funcionHash(const K& clave) const {
        // Usa el hash estándar de C++ para la clave K y aplica el módulo de la capacidad
        // Asegura que el tipo K tiene una especialización de std::hash<K> disponible.
        size_t hash_val = std::hash<K>{}(clave);
        return hash_val % capacidad; // Asegura que el índice está dentro de [0, capacidad-1]
    }

    template <typename K, typename V>
    size_t TablaHash<K, V>::funcionHashSecundaria(const K& clave) const {
        // Segunda función hash para Doble Hash.
        // Debe ser diferente de la primera y crucialmente NUNCA retornar 0 para evitar bucles infinitos en el sondeo.
        // Una forma común: un primo P > 1 (a menudo capacidad-2 si capacidad es primo) - (hash % P).
        // O simplemente: 1 + (hash % (capacidad - 2)).
        // Requiere que capacidad >= 2. Si capacidad es muy pequeña, esta función debe ser revisada.
        size_t hash2_val = std::hash<K>{}(clave);
        if (capacidad <= 2) return 1; // Fallback seguro, aunque encontrarSiguientePrimo asegura > CAPACIDAD_MINIMA >= 10
        // Queremos un salto entre 1 y capacidad-1.
        // Usar un primo más pequeño (como capacidad-2) para el módulo ayuda a evitar agrupamientos.
        // Ejemplo: un primo P < capacidad, return P - (hash2_val % P).
        // Si capacidad es primo, capacidad-2 podría ser una buena opción si capacidad-2 > 1.
        size_t salto = 1 + (hash2_val % (capacidad - 2)); // Salto entre 1 y capacidad-2 + 1 = capacidad-1
        if (debugMode) std::cout << "[DEBUG] funcionHashSecundaria para clave " << any_to_string(clave) << " -> salto " << salto << std::endl;
        return salto;
    }

// --- Funciones Auxiliares ---
    template <typename K, typename V>
    float TablaHash<K, V>::factorCarga() const {
        if (capacidad == 0) return 0.0f;
        return static_cast<float>(elementos) / capacidad;
    }

    template <typename K, typename V>
    float TablaHash<K, V>::factorCargaEfectivo() const {
        if (capacidad == 0) return 0.0f;
        // Para métodos de sondeo, la carga efectiva incluye elementos eliminados que ocupan espacio en la secuencia de sondeo
        return static_cast<float>(elementos + elementos_eliminados) / capacidad;
    }

    template <typename K, typename V>
    void TablaHash<K, V>::mostrar() const {
        for (const auto& par : *this) {
            std::cout << "(" << par.first << ", " << par.second << ")\n";
        }
    }

    template <typename K, typename V>
    void TablaHash<K, V>::mostrarDistribucion() const {
        if (metodo == ENCADENAMIENTO) {
            for (size_t i = 0; i < listas.size(); ++i) {
                std::cout << "[" << i << "]: ";
                for (const auto& par : listas[i]) {
                    std::cout << "(" << par.first << "," << par.second << ") ";
                }
                std::cout << "\n";
            }
        } else {
            for (size_t i = 0; i < tabla.size(); ++i) {
                if (tabla[i].ocupado && !tabla[i].eliminado) {
                    std::cout << "[" << i << "]: (" << tabla[i].clave << "," << tabla[i].valor << ")\n";
                } else {
                    std::cout << "[" << i << "]:\n";
                }
            }
        }
    }

    template <typename K, typename V>
    void TablaHash<K, V>::rehash(size_t nuevaCapacidad) {
        stats.rehashes++;
        if (debugMode) {
            std::cout << "[DEBUG] Realizando rehash. Capacidad actual: " << capacidad
                      << ", Elementos validos: " << elementos
                      << ", Elementos eliminados: " << elementos_eliminados
                      << ", Nueva capacidad deseada: " << nuevaCapacidad << std::endl;
        }

        // Asegurar que la nueva capacidad sea prima y cumpla el mínimo
        size_t proxPrimo = encontrarSiguientePrimo(std::max(nuevaCapacidad, CAPACIDAD_MINIMA));

        // Crear estructuras temporales con la nueva capacidad
        std::vector<HashEntry> nueva_tabla;
        std::vector<std::list<std::pair<K, V>>> nuevas_listas;

        if (metodo != ENCADENAMIENTO) {
            nueva_tabla.resize(proxPrimo); // Inicializa con HashEntry por defecto (no ocupado, no eliminado)
        } else {
            nuevas_listas.resize(proxPrimo);
        }

        // Guardar datos viejos para re-insertar
        // Usamos el iterador para recorrer de forma genérica y obtener solo elementos válidos
        std::vector<std::pair<K, V>> elementos_antiguos;
        elementos_antiguos.reserve(elementos); // Reservar espacio (optimización)

        // Recorrer la tabla actual usando el iterador para recopilar elementos válidos
        for(const auto& par : *this) { // Utiliza los métodos begin() y end() del iterador
            elementos_antiguos.push_back(par);
        }

        // Actualizar la tabla principal y capacidad ANTES de re-insertar
        capacidad = proxPrimo;
        elementos = 0; // Se recalculará durante la re-inserción
        elementos_eliminados = 0; // Se reseteará durante la re-inserción

        // Limpiar estructuras viejas y asignar las nuevas (mover para eficiencia)
        tabla.clear();
        listas.clear();
        if (metodo != ENCADENAMIENTO) {
            tabla = std::move(nueva_tabla); // Mover la nueva tabla vacía/reinicializada
        } else {
            listas = std::move(nuevas_listas); // Mover las nuevas listas vacías
        }

        // Re-insertar todos los elementos antiguos en la nueva tabla con la nueva capacidad.
        // Durante esta re-inserción, los factores de carga se volverán a chequear,
        // pero idealmente la nueva capacidad es lo suficientemente grande para que no ocurra otro rehash inmediato.
        if (debugMode) std::cout << "[DEBUG] Re-insertando " << elementos_antiguos.size() << " elementos en la nueva tabla.\n";
        for (const auto& par : elementos_antiguos) {
            // Llama a insertar. La lógica de insertar debe manejar el incremento de 'elementos'
            // y contar colisiones con la NUEVA estructura y funciones hash.
            insertar(par.first, par.second);
        }

        if (debugMode) {
            std::cout << "[DEBUG] Rehash completado. Nueva capacidad: " << capacidad
                      << ", Elementos validos re-insertados: " << elementos << std::endl;
        }
    }

// Funciones auxiliares para encontrar el siguiente primo
    template <typename K, typename V>
    bool TablaHash<K, V>::esPrimo(size_t n) const {
        if (n <= 1) return false;
        if (n <= 3) return true;
        if (n % 2 == 0 || n % 3 == 0) return false;
        // Iterar solo sobre impares > 3
        for (size_t i = 5; i * i <= n; i = i + 6) {
            if (n % i == 0 || n % (i + 2) == 0) return false;
        }
        return true;
    }

    template <typename K, typename V>
    size_t TablaHash<K, V>::encontrarSiguientePrimo(size_t n) const {
        if (n <= CAPACIDAD_MINIMA) return CAPACIDAD_MINIMA; // Asegurar capacidad mínima
        size_t primo = n;
        // Asegurarse de empezar desde un impar si n es par y mayor que 2
        if (primo % 2 == 0 && primo > 2) {
            primo++;
        }
        while (!esPrimo(primo)) {
            primo += 2; // Saltar de 2 en 2 (solo impares)
        }
        return primo;
    }


// --- Funciones de Ayuda para Serialización ---
    template <typename K, typename V>
    template <typename T>
    std::string TablaHash<K, V>::any_to_string(const T& valor) {
        std::stringstream ss;
        ss << valor;
        return ss.str();
    }

    template <typename K, typename V>
    template <typename T>
    T TablaHash<K, V>::string_to_any(const std::string& str) const {
        std::stringstream ss(str);
        T valor;
        ss >> valor;
        // Verifica si la conversión falló O si quedan caracteres no consumidos en el stream
        // Esto detecta casos donde, por ejemplo, intentas leer un int de "123abc".
        if (ss.fail() || ss.rdbuf()->in_avail() != 0) {
            throw std::runtime_error("Error de conversion: No se pudo convertir '" + str + "' al tipo deseado.");
        }
        return valor;
    }


// --- Métodos Públicos Principales (Implementaciones de Ejemplo) ---

    template <typename K, typename V>
    bool TablaHash<K, V>::insertar(const K& clave, const V& valor) {
        stats.inserciones++;
        size_t indice_inicial = funcionHash(clave); // Índice inicial calculado por la función hash principal

        if (metodo == ENCADENAMIENTO) {
            // ----- Lógica para Encadenamiento -----
            // Buscar si la clave ya existe en la lista en el índice_inicial
            for (const auto& par : listas[indice_inicial]) {
                if (par.first == clave) {
                    // Clave duplicada: ya existe en la tabla. No inserta.
                    if (debugMode) std::cout << "[DEBUG] Insertar: Clave " << any_to_string(clave) << " duplicada (Encadenamiento).\n";
                    return false; // Indica que no se insertó una nueva clave
                }
            }
            // Si el bucle termina, la clave no existe en la lista. Insertar.
            // Se considera colisión si la lista en el índice_inicial no estaba vacía ANTES de insertar este elemento.
            if (!listas[indice_inicial].empty()) {
                stats.colisiones++;
            }
            listas[indice_inicial].push_back({clave, valor}); // Insertar al final de la lista
            elementos++; // Incrementar el contador de elementos válidos (es una nueva clave)

            // Verificar si necesita rehash DESPUÉS de la inserción exitosa de un NUEVO elemento
            if (factorCarga() > factorCargaMax) {
                rehash(capacidad * 2); // Ejemplo simple: duplicar la capacidad
            }
            if (debugMode) std::cout << "[DEBUG] Insertar: Clave " << any_to_string(clave) << " insertada en indice " << indice_inicial << " (Encadenamiento).\n";
            return true; // Inserción exitosa de una nueva clave

        } else {
            // ----- Lógica para Métodos de Sondeo (Sondeo Lineal como ejemplo base) -----
            // Deberás adaptar la lógica de cálculo del 'indice' dentro del bucle para
            // Sondeo Cuadrático y Doble Hash.
            // La lógica de manejo de 'ocupado', 'eliminado', 'elementos' y 'elementos_eliminados'
            // es similar para todos los métodos de sondeo.

            size_t indice_actual = indice_inicial;
            size_t i = 0; // Contador/paso del sondeo
            size_t primer_eliminado_encontrado = (size_t)-1; // Usado para reutilizar slots eliminados

            // Bucle de sondeo: Continuar mientras la ranura actual esté ocupada O marcada como eliminada.
            // El bucle termina cuando encontramos una ranura realmente libre (!ocupado && !eliminado).
            while (tabla[indice_actual].ocupado || tabla[indice_actual].eliminado) {

                // Si la ranura está ocupada (y no eliminada) y la clave coincide, es una clave duplicada.
                if (tabla[indice_actual].ocupado && !tabla[indice_actual].eliminado && tabla[indice_actual].clave == clave) {
                    if (debugMode) std::cout << "[DEBUG] Insertar: Clave " << any_to_string(clave) << " duplicada (" << nombreMetodo() << ").\n";
                    return false; // Clave duplicada, no inserta.
                }

                // Si encontramos una ranura marcada como eliminada y es la primera que vemos, la registramos.
                // Podemos reutilizar un slot eliminado si no encontramos la clave duplicada y terminamos en un slot libre al final.
                if (tabla[indice_actual].eliminado && primer_eliminado_encontrado == (size_t)-1) {
                    primer_eliminado_encontrado = indice_actual;
                }

                // AVANZAR EL ÍNDICE DE SONDEO - AQUÍ CAMBIA LA LÓGICA SEGÚN EL MÉTODO
                i++; // Incrementar el paso del sondeo
                if (metodo == SONDEO_LINEAL) {
                    indice_actual = (indice_inicial + i) % capacidad;
                    if (debugMode) std::cout << "[DEBUG] Insertar(S. Lineal): Sondeando paso " << i << " -> indice " << indice_actual << "\n";
                } else if (metodo == SONDEO_CUADRATICO) {
                    // TODO: Lógica para SONDEO_CUADRATICO: indice_actual = (indice_inicial + i*i) % capacidad;
                    // Asegúrate de manejar el cálculo i*i para evitar desbordamiento si 'i' se vuelve grande.
                    // Es mejor usar un tipo más grande como 'long long' para i*i antes de aplicar el módulo.
                    long long paso_cuadratico = static_cast<long long>(i) * i;
                    indice_actual = (indice_inicial + paso_cuadratico) % capacidad;
                    if (debugMode) std::cout << "[DEBUG] Insertar(S. Cuadratico): Sondeando paso " << i << " (i*i=" << paso_cuadratico << ") -> indice " << indice_actual << "\n";

                } else if (metodo == DOBLE_HASH) {
                    // TODO: Lógica para DOBLE_HASH: indice_actual = (indice_inicial + i * funcionHashSecundaria(clave)) % capacidad;
                    size_t salto = funcionHashSecundaria(clave); // Calcular el salto usando la segunda función hash
                    indice_actual = (indice_inicial + i * salto) % capacidad;
                    if (debugMode) std::cout << "[DEBUG] Insertar(Doble Hash): Sondeando paso " << i << " (salto=" << salto << ") -> indice " << indice_actual << "\n";
                }

                // Contar colisiones: cada vez que la ranura que intentamos acceder NO es la inicial y está ocupada/eliminada
                // Hay varias formas de contar colisiones. Una es contar cada paso del sondeo después del primero que encuentra una ranura ocupada/eliminada.
                stats.colisiones++; // Contamos cada paso adicional como una colisión

                // DETECCIÓN DE CICLO: Si el contador de pasos 'i' alcanza o supera la capacidad, algo va mal
                // (tabla llena, o problema con el sondeo). En teoría, con factor de carga < 1 y sondeo correcto,
                // deberías encontrar una ranura libre o eliminada antes de esto.
                if (i >= capacidad) {
                    // Esto no debería pasar si el factor de carga efectivo es < 1.
                    // Si ocurre, indica que la tabla está lógicamente llena (aunque haya slots eliminados)
                    // o hay un problema con la función hash/sondeo.
                    if (debugMode) std::cerr << "[ERROR] Insertar: Ciclo de sondeo detectado o limite excedido para clave " << any_to_string(clave) << " (" << nombreMetodo() << "). Tabla llena o error.\n";
                    // Podrías forzar un rehash aquí si crees que la tabla está atascada
                    // rehash(capacidad + 1); // Intenta crecer
                    return false; // No se pudo insertar (tabla lógicamente llena o error)
                }
            }

            // Si el bucle while terminó, significa que encontramos una ranura libre (!ocupado && !eliminado)
            // O encontramos la clave duplicada (que ya se manejó dentro del bucle).
            // Si encontramos slots eliminados durante el sondeo (primer_eliminado_encontrado != -1),
            // es más eficiente reutilizar el *primer* slot eliminado encontrado en lugar del slot libre actual.
            size_t indice_para_insercion = (primer_eliminado_encontrado != (size_t)-1) ? primer_eliminado_encontrado : indice_actual;

            // Insertar el nuevo par clave-valor en la ranura encontrada/reutilizada
            tabla[indice_para_insercion].clave = clave;
            tabla[indice_para_insercion].valor = valor;
            tabla[indice_para_insercion].ocupado = true; // Marcar como ocupado

            // Si reutilizamos un slot que estaba marcado como eliminado
            if (tabla[indice_para_insercion].eliminado) {
                tabla[indice_para_insercion].eliminado = false; // Ya no está eliminado
                elementos_eliminados--; // Decrementar el contador de eliminados
                elementos++; // Incrementar el contador de elementos válidos
                if (debugMode) std::cout << "[DEBUG] Insertar: Clave " << any_to_string(clave) << " reutilizo slot eliminado en índice " << indice_para_insercion << " (" << nombreMetodo() << ").\n";
            } else {
                // Si insertamos en un slot que estaba realmente vacío (!ocupado && !eliminado)
                elementos++; // Incrementar el contador de elementos válidos
                // elementos_eliminados no cambia
                if (debugMode) std::cout << "[DEBUG] Insertar: Clave " << any_to_string(clave) << " insertada en slot vacío " << indice_para_insercion << " (" << nombreMetodo() << ").\n";
            }


            // Verificar si necesita rehash DESPUÉS de la inserción exitosa (de una NUEVA clave)
            // Para métodos de sondeo, es mejor usar el factor de carga efectivo para decidir el rehash.
            if (factorCargaEfectivo() > factorCargaMax) {
                rehash(capacidad * 2); // Ejemplo simple: duplicar la capacidad
            }

            return true; // Inserción exitosa de una nueva clave
        }
    }

    template <typename K, typename V>
    std::optional<V> TablaHash<K, V>::buscar(const K& clave) const {
        stats.busquedas++;
        size_t indice_inicial = funcionHash(clave); // Índice inicial

        if (metodo == ENCADENAMIENTO) {
            // ----- Lógica para Encadenamiento -----
            // Buscar en la lista en el índice calculado
            for (const auto& par : listas[indice_inicial]) {
                if (par.first == clave) {
                    // Clave encontrada en la lista
                    if (debugMode) std::cout << "[DEBUG] Buscar: Clave " << any_to_string(clave) << " encontrada (Encadenamiento).\n";
                    return par.second; // Retorna el valor envuelto en optional
                }
            }
            // Si el bucle termina, la clave no está en la lista
            if (debugMode) std::cout << "[DEBUG] Buscar: Clave " << any_to_string(clave) << " no encontrada (Encadenamiento).\n";
            return std::nullopt; // Clave no encontrada

        } else {
            // ----- Lógica para Métodos de Sondeo (Sondeo Lineal como ejemplo base) -----
            size_t indice_actual = indice_inicial;
            size_t i = 0; // Contador/paso del sondeo

            // Bucle de sondeo: Continuar mientras la ranura actual NO sea una ranura realmente vacía (!ocupado).
            // Es crucial pasar por las ranuras marcadas como eliminadas, ya que la clave buscada podría haber
            // colisionado con ellas durante la inserción original y estar ubicada más adelante en la secuencia de sondeo.
            while (tabla[indice_actual].ocupado || tabla[indice_actual].eliminado) {

                // Si la ranura está ocupada (y no eliminada) y la clave coincide, encontramos.
                if (tabla[indice_actual].ocupado && !tabla[indice_actual].eliminado && tabla[indice_actual].clave == clave) {
                    if (debugMode) std::cout << "[DEBUG] Buscar: Clave " << any_to_string(clave) << " encontrada en índice " << indice_actual << " (" << nombreMetodo() << ").\n";
                    return tabla[indice_actual].valor; // Clave encontrada
                }

                stats.colisiones++; // Cada paso de sondeo después del primero cuenta como colisión

                // AVANZAR EL ÍNDICE DE SONDEO - AQUÍ CAMBIA LA LÓGICA SEGÚN EL MÉTODO
                i++; // Incrementar el paso del sondeo
                if (metodo == SONDEO_LINEAL) {
                    indice_actual = (indice_inicial + i) % capacidad;
                    if (debugMode) std::cout << "[DEBUG] Buscar(S. Lineal): Sondeando paso " << i << " -> indice " << indice_actual << "\n";
                } else if (metodo == SONDEO_CUADRATICO) {
                    // TODO: Lógica para SONDEO_CUADRATICO: indice_actual = (indice_inicial + i*i) % capacidad;
                    long long paso_cuadratico = static_cast<long long>(i) * i;
                    indice_actual = (indice_inicial + paso_cuadratico) % capacidad;
                    if (debugMode) std::cout << "[DEBUG] Buscar(S. Cuadratico): Sondeando paso " << i << " (i*i=" << paso_cuadratico << ") -> indice " << indice_actual << "\n";
                } else if (metodo == DOBLE_HASH) {
                    // TODO: Lógica para DOBLE_HASH: indice_actual = (indice_inicial + i * funcionHashSecundaria(clave)) % capacidad;
                    size_t salto = funcionHashSecundaria(clave);
                    indice_actual = (indice_inicial + i * salto) % capacidad;
                    if (debugMode) std::cout << "[DEBUG] Buscar(Doble Hash): Sondeando paso " << i << " (salto=" << salto << ") -> indice " << indice_actual << "\n";
                }

                // DETECCIÓN DE FINAL EN SONDEO: Si el contador de pasos 'i' alcanza o supera la capacidad,
                // hemos recorrido toda la tabla o estamos en un ciclo.
                if (i >= capacidad) {
                    if (debugMode) std::cout << "[DEBUG] Buscar: Ciclo de sondeo o limite excedido para clave " << any_to_string(clave) << " (" << nombreMetodo() << "). Clave no encontrada.\n";
                    return std::nullopt; // Clave no encontrada
                }
            }

            // Si el bucle while terminó, significa que encontramos una ranura REALMENTE vacía (!ocupado && !eliminado).
            // Esto implica que la clave nunca fue insertada o ya fue eliminada de forma permanente (lo cual no hacemos).
            if (debugMode) std::cout << "[DEBUG] Buscar: Ranura vacía encontrada en índice " << indice_actual << " (" << nombreMetodo() << "). Clave no encontrada.\n";
            return std::nullopt; // Clave no encontrada
        }
    }

    template <typename K, typename V>
    bool TablaHash<K, V>::eliminar(const K& clave) {
        stats.eliminaciones++;
        size_t indice_inicial = funcionHash(clave); // Índice inicial

        if (metodo == ENCADENAMIENTO) {
            // ----- Lógica para Encadenamiento -----
            // Buscar y eliminar de la lista en el índice_inicial
            for (auto it = listas[indice_inicial].begin(); it != listas[indice_inicial].end(); ++it) {
                if (it->first == clave) {
                    it = listas[indice_inicial].erase(it); // Eliminar el elemento de la lista. 'erase' retorna el iterador al siguiente elemento.
                    elementos--; // Decrementar el contador de elementos válidos
                    if (debugMode) std::cout << "[DEBUG] Eliminar: Clave " << any_to_string(clave) << " eliminada (Encadenamiento).\n";

                    // Opcional: Rehash hacia abajo si el factor de carga es muy bajo después de eliminar.
                    // Esto ayuda a ahorrar memoria pero tiene un costo de rendimiento.
                    // Decide con qué frecuencia y bajo qué condiciones quieres hacer rehash hacia abajo.
                    // if (elementos > CAPACIDAD_MINIMA && factorCarga() < factorCargaMin) {
                    //     rehash(capacidad / 2); // Reducir capacidad a la mitad (buscando el siguiente primo)
                    // }

                    return true; // Eliminación exitosa
                }
            }
            // Si el bucle termina, la clave no está en la lista.
            if (debugMode) std::cout << "[DEBUG] Eliminar: Clave " << any_to_string(clave) << " no encontrada (Encadenamiento).\n";
            return false; // Clave no encontrada

        } else {
            // ----- Lógica para Métodos de Sondeo (Sondeo Lineal como ejemplo base) -----
            size_t indice_actual = indice_inicial;
            size_t i = 0; // Contador/paso del sondeo

            // Bucle de sondeo: Buscar la clave o una ranura REALMENTE vacía (!ocupado && !eliminado).
            // Similar a la búsqueda, debemos pasar por ranuras eliminadas.
            while (tabla[indice_actual].ocupado || tabla[indice_actual].eliminado) {

                // Si la ranura está ocupada (no eliminada) y la clave coincide, encontramos el elemento a eliminar.
                if (tabla[indice_actual].ocupado && !tabla[indice_actual].eliminado && tabla[indice_actual].clave == clave) {
                    // En lugar de borrar, marcamos la ranura como eliminada.
                    tabla[indice_actual].ocupado = true; // Mantiene ocupado lógicamente el slot en la secuencia de sondeo
                    tabla[indice_actual].eliminado = true; // Marca como eliminado (disponible para futuras inserciones)
                    elementos--; // Decrementar el contador de elementos válidos
                    elementos_eliminados++; // Incrementar el contador de elementos eliminados

                    if (debugMode) std::cout << "[DEBUG] Eliminar: Clave " << any_to_string(clave) << " marcada como eliminada en índice " << indice_actual << " (" << nombreMetodo() << ").\n";

                    // Opcional: Rehash hacia abajo si el factor de carga EFECTIVO es muy bajo y hay muchos eliminados.
                    // Esto ayuda a "compactar" la tabla y reducir el impacto de los slots eliminados en el sondeo futuro.
                    // if (elementos > CAPACIDAD_MINIMA && factorCargaEfectivo() < factorCargaMin && elementos_eliminados > elementos) {
                    //    rehash(encontrarSiguientePrimo(elementos / factorCargaMax)); // Intentar una capacidad que ponga la carga cerca del max
                    // }

                    return true; // Eliminación exitosa (marcado)
                }

                stats.colisiones++; // Cada paso de sondeo después del primero cuenta como colisión

                // AVANZAR EL ÍNDICE DE SONDEO - AQUÍ CAMBIA LA LÓGICA SEGÚN EL MÉTODO
                i++; // Incrementar el paso del sondeo
                if (metodo == SONDEO_LINEAL) {
                    indice_actual = (indice_inicial + i) % capacidad;
                    if (debugMode) std::cout << "[DEBUG] Eliminar(S. Lineal): Sondeando paso " << i << " -> indice " << indice_actual << "\n";
                } else if (metodo == SONDEO_CUADRATICO) {
                    // TODO: Lógica para SONDEO_CUADRATICO: indice_actual = (indice_inicial + i*i) % capacidad;
                    long long paso_cuadratico = static_cast<long long>(i) * i;
                    indice_actual = (indice_inicial + paso_cuadratico) % capacidad;
                    if (debugMode) std::cout << "[DEBUG] Eliminar(S. Cuadratico): Sondeando paso " << i << " (i*i=" << paso_cuadratico << ") -> indice " << indice_actual << "\n";
                } else if (metodo == DOBLE_HASH) {
                    // TODO: Lógica para DOBLE_HASH: indice_actual = (indice_inicial + i * funcionHashSecundaria(clave)) % capacidad;
                    size_t salto = funcionHashSecundaria(clave);
                    indice_actual = (indice_inicial + i * salto) % capacidad;
                    if (debugMode) std::cout << "[DEBUG] Eliminar(Doble Hash): Sondeando paso " << i << " (salto=" << salto << ") -> indice " << indice_actual << "\n";
                }

                // DETECCIÓN DE FINAL EN SONDEO: Si el contador de pasos 'i' alcanza o supera la capacidad,
                // hemos recorrido toda la tabla sin encontrar la clave.
                if (i >= capacidad) {
                    if (debugMode) std::cout << "[DEBUG] Eliminar: Ciclo de sondeo o limite excedido para clave " << any_to_string(clave) << " (" << nombreMetodo() << "). Clave no encontrada.\n";
                    return false; // Clave no encontrada
                }
            }

            // Si el bucle while terminó, significa que encontramos una ranura REALMENTE vacía (!ocupado && !eliminado).
            // Esto implica que la clave no existe en la tabla en este momento.
            if (debugMode) std::cout << "[DEBUG] Eliminar: Ranura vacía encontrada en índice " << indice_actual << " (" << nombreMetodo() << "). Clave no encontrada.\n";
            return false; // Clave no encontrada
        }
    }

// Implementación de mostrar, mostrarEstadisticas, mostrarDistribucion,
// cambiarMetodo, configurarFactorCarga, cargarDatosPrueba, vaciar,
// guardarEnArchivo, cargarDesdeArchivo, toggleDebugMode, nombreMetodo
// ... estas funciones ya tenían una estructura básica razonable en tu código original
// o fueron completadas en la respuesta anterior. Las mantendremos como estaban
// o con las mejoras ya sugeridas (como el manejo de errores en string_to_any).

// --- Implementación del Iterador ---

    template <typename K, typename V>
    TablaHash<K, V>::Iterator::Iterator(const TablaHash<K, V>* t, size_t inicio_indice)
            : tabla_ptr(t), indice_actual(inicio_indice) {
        if (t->metodo == ENCADENAMIENTO && inicio_indice < t->capacidad) {
            list_it = t->listas[inicio_indice].begin();
        }
    }


    template <typename K, typename V>
    void TablaHash<K, V>::Iterator::encontrarSiguienteElemento() {
        if (indice_actual >= tabla->capacidad) return;

        if (tabla->metodo == ENCADENAMIENTO) {
            ++list_it;
            while (indice_actual < tabla->capacidad && list_it == tabla->listas[indice_actual].end()) {
                ++indice_actual;
                if (indice_actual < tabla->capacidad)
                    list_it = tabla->listas[indice_actual].begin();
            }
        } else {
            do {
                ++indice_actual;
            } while (indice_actual < tabla->capacidad &&
                     (!tabla->tabla[indice_actual].ocupado || tabla->tabla[indice_actual].eliminado));
        }
    }



    // Buscar la siguiente lista no vacía si el iterador de lista actual llegó al final
            while (indice_actual < tabla->capacidad && list_it == tabla->listas[indice_actual].end()) {
                indice_actual++; // Mover al siguiente índice en el vector de listas
                if (indice_actual < tabla->capacidad) {
                    list_it = tabla->listas[indice_actual].begin(); // Iniciar el iterador de lista en la nueva lista
                    // Si la nueva lista está vacía, el bucle while continuará.
                }
            }
            // Si indice_actual llegó a capacidad, hemos llegado al final global
            if (indice_actual >= tabla->capacidad) {
                // Opcional: Poner list_it a un estado inválido o end si quieres ser explícito
                // list_it = {}; // O alguna forma de indicar que ya no apunta a una lista válida
            }

        } else {
            // Lógica para Métodos de Sondeo: Buscar el próximo slot ocupado y no eliminado.
            indice_actual++; // Empezar a buscar desde el siguiente slot al actual

            while (indice_actual < tabla->capacidad) {
                // Si encontramos un slot ocupado Y NO eliminado, hemos encontrado el siguiente elemento válido.
                if (tabla->tabla[indice_actual].ocupado && !tabla->tabla[indice_actual].eliminado) {
                    return; // Encontrado, el iterador ahora apunta a esta posición 'indice_actual'
                }
                indice_actual++; // Si no es válido (vacío o eliminado), pasar al siguiente slot
            }
            // Si el bucle termina, indice_actual es ahora igual a capacidad, lo que indica el final de la tabla.
        }
    }

    template <typename K, typename V>
    std::pair<K, V> TablaHash<K, V>::Iterator::operator*() const {
        // No se puede desreferenciar un iterador que apunta al final ('end').
        if (indice_actual >= tabla->capacidad) {
            throw std::out_of_range("Intento de desreferenciar un iterador que apunta al final de la tabla.");
        }

        if (tabla->metodo == ENCADENAMIENTO) {
            // Para encadenamiento, devolver el par al que apunta el iterador de lista.
            // Asegúrate de que list_it es válido (no end() de la lista actual).
            if (list_it == tabla->listas[indice_actual].end()) {
                throw std::runtime_error("Error interno del iterador (Encadenamiento): list_it apunta a end() en un indice valido.");
            }
            return *list_it; // Devuelve una copia del par clave-valor de la lista
        } else {
            // Para sondeo, devolver el par clave-valor del slot actual en el vector 'tabla'.
            // Asegúrate de que el slot está ocupado y no eliminado (el iterador debería garantizar esto).
            if (!tabla->tabla[indice_actual].ocupado || tabla->tabla[indice_actual].eliminado) {
                throw std::runtime_error("Error interno del iterador (Sondeo): Apuntando a slot invalido (vacio/eliminado) en un indice valido.");
            }
            return {tabla->tabla[indice_actual].clave, tabla->tabla[indice_actual].valor}; // Devuelve una copia del par clave-valor del vector
        }
    }

    template <typename K, typename V>
    typename TablaHash<K, V>::Iterator& TablaHash<K, V>::Iterator::operator++() {
        encontrarSiguienteElemento(); // Avanza al siguiente elemento válido
        return *this; // Devuelve una referencia a sí mismo (el iterador ya avanzado)
    }

// Implementar el post-incremento si es necesario (opcional para range-based for)
// template <typename K, typename V>
// typename TablaHash<K, V>::Iterator TablaHash<K, V>::Iterator::operator++(int) {
//    Iterator temp = *this; // Hacer una copia del estado actual
//    ++(*this); // Avanzar el iterador actual usando el pre-incremento
//    return temp; // Devolver la copia del estado antiguo
// }


    template <typename K, typename V>
    bool TablaHash<K, V>::Iterator::operator==(const Iterator& otro) const {
        // Dos iteradores son iguales si pertenecen a la misma tabla
        // Y si ambos apuntan a la misma posición.
        // La "posición" se define por 'indice_actual' y, para encadenamiento, por 'list_it'.
        // Si ambos 'indice_actual' son >= capacidad, ambos son 'end'.
        if (tabla != otro.tabla) return false;

        // Si ambos iteradores están en la posición 'end'
        if (indice_actual >= tabla->capacidad && otro.indice_actual >= tabla->capacidad) {
            return true;
        }
        // Si solo uno está en la posición 'end'
        if (indice_actual >= tabla->capacidad || otro.indice_actual >= tabla->capacidad) {
            return false;
        }

        // Ambos iteradores están apuntando a posiciones DENTRO del rango [0, capacidad-1]
        if (tabla->metodo == ENCADENAMIENTO) {
            // Para encadenamiento, la igualdad requiere coincidencia en el índice del vector Y en el iterador de lista.
            return indice_actual == otro.indice_actual && list_it == otro.list_it;
        } else {
            // Para sondeo, la igualdad solo requiere coincidencia en el índice del vector.
            return indice_actual == otro.indice_actual;
        }
    }

    template <typename K, typename V>
    bool TablaHash<K, V>::Iterator::operator!=(const Iterator& otro) const {
        return !(*this == otro);
    }

    template <typename K, typename V>
    typename TablaHash<K, V>::Iterator TablaHash<K, V>::begin() const {
        Iterator it(this, 0);
        if (it.indice_actual < capacidad &&
            ((metodo != ENCADENAMIENTO &&
              (!tabla[it.indice_actual].ocupado || tabla[it.indice_actual].eliminado)) ||
             (metodo == ENCADENAMIENTO &&
              it.list_it == listas[it.indice_actual].end()))) {
            it.encontrarSiguienteElemento();
        }
        return it;
    }

    template <typename K, typename V>
    typename TablaHash<K, V>::Iterator TablaHash<K, V>::end() const {
        return Iterator(this, capacidad);
    }


// --- Implementaciones adicionales de métodos públicos ---

    template <typename K, typename V>
    void TablaHash<K, V>::mostrarEstadisticas() const {
        std::cout << "\n--- Estadísticas de la Tabla Hash ---\n";
        std::cout << "Método de Colisión: " << nombreMetodo() << "\n";
        std::cout << "Capacidad Actual: " << capacidad << "\n";
        std::cout << "Elementos Válidos (insertados - eliminados): " << elementos << "\n";
        if (metodo != ENCADENAMIENTO) {
            std::cout << "Elementos Marcados como Eliminados: " << elementos_eliminados << "\n";
        }
        std::cout << "Factor de Carga Actual (validos/capacidad): " << factorCarga() << "\n";
        if (metodo != ENCADENAMIENTO) {
            std::cout << "Factor de Carga Efectivo ((validos+eliminados)/capacidad): " << factorCargaEfectivo() << "\n";
        }
        std::cout << "Factor de Carga Máximo Permitido: " << factorCargaMax << "\n";
        std::cout << "Factor de Carga Mínimo Permitido: " << factorCargaMin << "\n";
        std::cout << "Inserciones Intentadas: " << stats.inserciones << "\n";
        std::cout << "Búsquedas Intentadas: " << stats.busquedas << "\n";
        std::cout << "Eliminaciones Intentadas: " << stats.eliminaciones << "\n";
        // La definición de colisión puede variar ligeramente entre métodos.
        // Para sondeo, suele ser cada paso del sondeo después del primero.
        // Para encadenamiento, es cuando llegas a un índice con una lista no vacía.
        std::cout << "Colisiones Contabilizadas durante Insercion/Busqueda/Eliminacion (definicion depende del metodo): " << stats.colisiones << "\n";
        std::cout << "Rehashes Realizados: " << stats.rehashes << "\n";
        std::cout << "--- Fin de Estadísticas ---\n";
    }


    template <typename K, typename V>
    void TablaHash<K, V>::cambiarMetodo(MetodoColision nuevoMetodo) {
        if (metodo == nuevoMetodo) {
            if (debugMode) std::cout << "[DEBUG] CambiarMetodo: El método ya es " << nombreMetodo() << ". No se hace nada.\n";
            return; // No hacer nada si el método es el mismo
        }

        if (debugMode) std::cout << "[DEBUG] CambiarMetodo: Cambiando de " << nombreMetodo() << " a " << nombreMetodo() << "...\n";


        // Es necesario rehacer la tabla desde cero para el nuevo método
        // Mantener los elementos actuales y reinsertarlos.
        // Usamos el iterador para obtener una copia de los elementos válidos actuales.
        std::vector<std::pair<K, V>> elementos_actuales;
        elementos_actuales.reserve(elementos); // Optimización: reservar espacio
        for(const auto& par : *this) { // Utiliza begin() y end() para iterar sobre elementos válidos
            elementos_actuales.push_back(par);
        }

        // Vaciar las estructuras de datos antiguas y resetear contadores lógicos.
        tabla.clear();
        listas.clear();
        elementos = 0;
        elementos_eliminados = 0;
        // No reseteamos stats.

        // Asignar el nuevo método ANTES de redimensionar e insertar
        metodo = nuevoMetodo;

        // Redimensionar las estructuras de datos para el nuevo método
        if (metodo != ENCADENAMIENTO) {
            tabla.resize(capacidad); // Redimensionar el vector para sondeo
            if (debugMode) std::cout << "[DEBUG] Redimensionando vector 'tabla' a capacidad " << capacidad << " para metodo de sondeo.\n";
        } else {
            listas.resize(capacidad); // Redimensionar el vector de listas para encadenamiento
            if (debugMode) std::cout << "[DEBUG] Redimensionando vector 'listas' a capacidad " << capacidad << " para encadenamiento.\n";
        }


        // Re-insertar los elementos que teníamos con el nuevo método.
        // Esto utilizará las funciones hash y la lógica de inserción del nuevo método.
        // Podría desencadenar un rehash si la capacidad actual no es suficiente para la carga.
        if (debugMode) std::cout << "[DEBUG] Reinsertando " << elementos_actuales.size() << " elementos con el nuevo método...\n";
        for (const auto& par : elementos_actuales) {
            insertar(par.first, par.second); // Llama al método insertar con la lógica ya cambiada
        }
        if (debugMode) std::cout << "[DEBUG] Elementos reinsertados. Total elementos validos: " << elementos << "\n";

        // Las estadísticas se mantienen para mostrar el historial de operaciones.
        // Si quisieras resetear las estadísticas al cambiar de método, lo harías aquí: stats.reset();
    }

// Implementación de configurarFactorCarga (ya mejorada en respuesta anterior)
    template <typename K, typename V>
    void TablaHash<K, V>::configurarFactorCarga(float nuevoFactorMax, float nuevoFactorMin) {
        // Validar rangos
        if (nuevoFactorMax < FACTOR_MAX_MINIMO || nuevoFactorMax > FACTOR_MAX_MAXIMO) {
            throw std::invalid_argument("Factor de carga maximo fuera de rango (0.4 - 0.95)");
        }
        if (nuevoFactorMin < FACTOR_MIN_MINIMO || nuevoFactorMin > FACTOR_MIN_MAXIMO) {
            throw std::invalid_argument("Factor de carga minimo fuera de rango (0.1 - 0.5)");
        }
        if (nuevoFactorMin >= nuevoFactorMax) {
            throw std::invalid_argument("Factor de carga minimo debe ser menor que el maximo");
        }

        factorCargaMax = nuevoFactorMax;
        factorCargaMin = nuevoFactorMin;

        if (debugMode) {
            std::cout << "[DEBUG] Factores de carga actualizados: Max=" << factorCargaMax << ", Min=" << factorCargaMin << std::endl;
        }

        // Opcional: Evaluar si necesita rehash inmediatamente después de cambiar los factores
        // si la carga actual ya excede el nuevo máximo o cae por debajo del nuevo mínimo
        // (especialmente importante para compactar si hay muchos eliminados en sondeo y el min bajó mucho).
        // if (metodo != ENCADENAMIENTO && factorCargaEfectivo() < factorCargaMin && elementos > 0 && elementos_eliminados > elementos) {
        //    // Ejemplo: Rehash hacia abajo si sondeo, carga efectiva baja, hay elementos y muchos eliminados
        //    rehash(encontrarSiguientePrimo(elementos / factorCargaMax)); // Intentar una capacidad que ponga la carga cerca del nuevo max
        // } else if (factorCarga() > factorCargaMax) {
        //     rehash(capacidad + 1); // Intentar crecer si ya excede el nuevo max (aunque la insercion/eliminacion lo deberia gatillar)
        // }
    }

// Implementación de cargarDatosPrueba (ya mejorada en respuesta anterior)
    template <typename K, typename V>
    void TablaHash<K, V>::cargarDatosPrueba(size_t cantidad) {
        // Implementación básica. Asume que K es int y V es std::string.
        // Si usas otros tipos, necesitarás especializar o modificar esta función.
        if constexpr (!std::is_same_v<K, int> || !std::is_same_v<V, std::string>) {
            std::cerr << "Advertencia: cargarDatosPrueba solo tiene implementacion basica para K=int, V=std::string.\n";
            return; // Salir si los tipos no coinciden con la implementacion de prueba
        }
        if (cantidad == 0) return;

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distrib(1, cantidad * 50); // Rango de claves más amplio para reducir colisiones iniciales


        if (debugMode) std::cout << "[DEBUG] Cargando " << cantidad << " datos de prueba...\n";
        size_t inserciones_exitosas = 0;
        for(size_t i = 0; i < cantidad; ++i) {
            int clave = distrib(gen);
            std::string valor = "valor_" + std::to_string(clave);
            // La función insertar se encargará del rehash si es necesario durante la carga.
            if (insertar(clave, valor)) {
                inserciones_exitosas++;
            }
        }
        if (debugMode) std::cout << "[DEBUG] Intentado cargar " << cantidad << " datos de prueba. Inserciones exitosas: " << inserciones_exitosas << std::endl;
    }


// Implementación de vaciar (ya mejorada en respuesta anterior)
    template <typename K, typename V>
    void TablaHash<K, V>::vaciar() {
        if (debugMode) std::cout << "[DEBUG] Vaciando tabla...\n";
        // Limpiar las estructuras de datos según el método
        if (metodo == ENCADENAMIENTO) {
            for (auto& lista : listas) {
                lista.clear();
            }
        } else {
            // Sondeo: Reinicializar el vector de entradas para marcar todo como vacío y no eliminado.
            // Usar resize(capacidad) invocando el constructor por defecto es la forma limpia.
            // Alternativa: Iterar y poner ocupado=false, eliminado=false en cada HashEntry.
            tabla.assign(capacidad, HashEntry()); // Reinicializa el vector con entradas vacías
        }
        elementos = 0;
        elementos_eliminados = 0;
        stats.reset(); // También resetear estadísticas
        if (debugMode) std::cout << "[DEBUG] Tabla vaciada y estadisticas reseteadas.\n";
    }


// Implementación de guardarEnArchivo (ya mejorada con any_to_string y manejo básico de errores)
    template <typename K, typename V>
    bool TablaHash<K, V>::guardarEnArchivo(const std::string& nombreArchivo) {
        std::ofstream archivo(nombreArchivo);
        if (!archivo.is_open()) {
            std::cerr << "Error al abrir archivo para guardar: " << nombreArchivo << std::endl;
            return false;
        }

        // Guardar metadatos básicos para poder reconstruir la tabla al cargar
        archivo << "Metodo:" << static_cast<int>(metodo) << "\n";
        archivo << "Capacidad:" << capacidad << "\n";
        // No es necesario guardar elementos y elementos_eliminados, se reconstruyen al reinsertar

        // Guardar cada par clave-valor válido usando el iterador
        if (debugMode) std::cout << "[DEBUG] Guardando tabla en " << nombreArchivo << ". Total elementos validos: " << elementos << "\n";
        for(const auto& par : *this) { // Utiliza el iterador begin/end
            try {
                // Usamos ';' como delimitador simple entre clave y valor. Asegúrate que ';' no aparezca en tus claves/valores serializados.
                archivo << any_to_string(par.first) << ";" << any_to_string(par.second) << "\n";
            } catch (const std::runtime_error& e) {
                std::cerr << "Error de serializacion: Fallo al convertir elemento a string para guardar: " << e.what() << ". Elemento omitido.\n";
                // Decide si quieres parar o continuar. Aquí continuamos.
            } catch (...) {
                std::cerr << "Error desconocido de serializacion al guardar elemento. Elemento omitido.\n";
            }
        }

        archivo.close();
        if (debugMode) std::cout << "[DEBUG] Tabla guardada exitosamente en " << nombreArchivo << ".\n";
        return true;
    }


// Implementación de cargarDesdeArchivo (ya mejorada con string_to_any, manejo básico de errores y metadatos)
    template <typename K, typename V>
    bool TablaHash<K, V>::cargarDesdeArchivo(const std::string& nombreArchivo) {
        std::ifstream archivo(nombreArchivo);
        if (!archivo.is_open()) {
            std::cerr << "Error al abrir archivo para cargar: " << nombreArchivo << std::endl;
            return false;
        }

        if (debugMode) std::cout << "[DEBUG] Cargando tabla desde " << nombreArchivo << "...\n";

        vaciar(); // Vaciar la tabla actual y resetear estadísticas antes de cargar

        std::string linea;
        // Leer metadatos (ejemplo básico, necesitas validación de formato robusta para un uso en producción)
        // Intenta leer el método de colisión
        if (std::getline(archivo, linea) && linea.rfind("Metodo:", 0) == 0) {
            try {
                int m = std::stoi(linea.substr(7));
                if (m >= ENCADENAMIENTO && m <= DOBLE_HASH) {
                    MetodoColision metodoCargado = static_cast<MetodoColision>(m);
                    // Cambiar al método cargado. Esto redimensionará y preparará las estructuras.
                    cambiarMetodo(metodoCargado); // Nota: cambiarMetodo hará rehash de 0 elementos, lo cual está bien aquí.
                    if (debugMode) std::cout << "[DEBUG] Metodo cargado del archivo: " << nombreMetodo() << ".\n";
                } else {
                    std::cerr << "Advertencia al cargar: Metodo de colision invalido (" << m << ") en archivo. Usando el metodo actual (" << nombreMetodo() << ").\n";
                }
            } catch (const std::exception& e) {
                std::cerr << "Advertencia al cargar: Error al parsear Metodo de colision en archivo (" << e.what() << "). Usando el metodo actual (" << nombreMetodo() << ").\n";
            }
        } else {
            std::cerr << "Advertencia al cargar: No se encontró la linea 'Metodo:' esperada o el archivo esta vacio. Usando el metodo actual (" << nombreMetodo() << ").\n";
            // Si no se pudo leer el método, no intentamos leer la capacidad con formato específico.
            // Nos quedamos con la capacidad inicial de la tabla.
        }

        // Intentar leer la capacidad (opcional, la inserción se encargará del rehash si es necesario)
        // Si no leímos el método, la próxima línea podría ser un elemento.
        // Podrías refactorizar esto para ser más robusto.
        if (std::getline(archivo, linea) && linea.rfind("Capacidad:", 0) == 0) {
            try {
                size_t c = std::stoul(linea.substr(10));
                // La capacidad cargada puede ser diferente de la capacidad actual.
                // Podrías forzar un rehash a esta capacidad después de leer todos los elementos,
                // o simplemente dejar que las inserciones lo manejen si la carga es alta.
                // Para simplificar, solo la registramos o la usamos si cambiamos el método antes.
                if (debugMode) std::cout << "[DEBUG] Capacidad leida del archivo: " << c << ".\n";
                // Si no se cambió el método (porque no se leyó correctamente),
                // considera si quieres redimensionar a esta capacidad *ahora* antes de insertar.
                // Ejemplo: if (metodo no cambió) rehash(c); // Riesgo si el archivo es malicioso o corrupto.
                // Es más seguro dejar que insertar haga el rehash si la carga lo requiere.

            } catch (const std::exception& e) {
                std::cerr << "Advertencia al cargar: Error al parsear Capacidad en archivo (" << e.what() << ").\n";
            }
        } else {
            std::cerr << "Advertencia al cargar: No se encontró la linea 'Capacidad:' esperada o es el primer elemento.\n";
            // Si no se encontró la capacidad, la última línea leída es en realidad el primer elemento.
            // Necesitas "devolver" esta línea para que el siguiente bucle la procese.
            // Esto se puede hacer con archive.seekg(pos_anterior);
            // O, más simple, procesar la línea actual si parece un elemento ANTES del bucle while.
            // Aquí, para simplificar, asumiremos que el formato Metodo/Capacidad está al inicio si existe.
        }


        // Leer y insertar elementos línea por línea
        // Si las líneas Metodo/Capacidad no se encontraron, el bucle empezará con la primera línea real.
        while (true) {
            // Si ya leímos una línea que no era Metodo/Capacidad, la procesamos ahora.
            // De lo contrario, leemos la siguiente línea.
            if (linea.empty() || linea.rfind("Metodo:", 0) == 0 || linea.rfind("Capacidad:", 0) == 0) {
                if (!std::getline(archivo, linea)) {
                    break; // Salir del bucle si no hay más líneas
                }
            }

            size_t delimitador_pos = linea.find(';');
            if (delimitador_pos == std::string::npos) {
                std::cerr << "Advertencia al cargar: Linea con formato incorrecto (sin ';') en archivo: '" << linea << "'. Saltando linea.\n";
                linea = ""; // Procesamos la línea actual, la vaciamos para leer la siguiente en la próxima iteración
                continue; // Saltar líneas mal formadas
            }

            std::string clave_str = linea.substr(0, delimitador_pos);
            std::string valor_str = linea.substr(delimitador_pos + 1);

            try {
                K clave = string_to_any<K>(clave_str);
                V valor = string_to_any<V>(valor_str);
                insertar(clave, valor); // Insertar el elemento cargado. 'insertar' maneja duplicados y rehash.
            } catch (const std::runtime_error& e) {
                std::cerr << "Error de conversion al cargar elemento '" << linea << "': " << e.what() << ". Elemento omitido.\n";
                // Decide si quieres parar o continuar. Aquí continuamos.
            } catch (...) {
                std::cerr << "Error desconocido al cargar elemento '" << linea << "'. Elemento omitido.\n";
                // Decide si quieres parar o continuar. Aquí continuamos.
            }
            linea = ""; // Procesamos la línea actual, la vaciamos para leer la siguiente en la próxima iteración
        }

        archivo.close();
        if (debugMode) std::cout << "[DEBUG] Tabla cargada desde " << nombreArchivo << ". Elementos validos: " << elementos << ".\n";
        return true;
    }


// Implementación de toggleDebugMode (ya en tu original/respuesta anterior)
    template <typename K, typename V>
    void TablaHash<K, V>::toggleDebugMode() {
        debugMode = !debugMode;
        std::cout << "Modo Debug: " << (debugMode ? "ON" : "OFF") << std::endl;
    }

// Implementación de nombreMetodo (ya en tu original/respuesta anterior)
    template <typename K, typename V>
    std::string TablaHash<K, V>::nombreMetodo() const {
        switch (metodo) {
            case ENCADENAMIENTO: return "Encadenamiento";
            case SONDEO_LINEAL: return "Sondeo Lineal";
            case SONDEO_CUADRATICO: return "Sondeo Cuadratico";
            case DOBLE_HASH: return "Doble Hash";
            default: return "Desconocido";
        }
    }


} // Cierra el namespace