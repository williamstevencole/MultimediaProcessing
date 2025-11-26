### PSEUDOCÓDIGO: Median Cut

#### FASE 1: PREPARACIÓN (Aplanar la imagen)

1.  **Recibir Datos:** Obtener puntero a la imagen cruda, ancho y alto.
2.  **Crear Lista de Píxeles:** Crear un vector vacío `pixels`.
3.  **Bucle de Aplanado:** Recorrer toda la imagen original:
    - Leer R, G, B.
    - Guardar **Índice Original** (`i`).
    - Insertar en el vector `pixels`.
4.  **Inicializar Cajas:** Crear vector `boxes` y agregar la **Caja 0** (que va desde `0` hasta el `total_pixeles`).

#### FASE 2: EL BUCLE DE CORTE (Divide y Vencerás)

**MIENTRAS** (`boxes.size() < num_colors`):

```
**A. Buscar la caja ganadora (La que peor se ve)**
1. Inicializar `max_range = -1` y `split_index = -1`.
2. **PARA CADA** caja en `boxes`:
    * Calcular Mín y Máx de Rojo, Verde y Azul dentro de esa caja.
    * Calcular rangos (`max - min`) para cada canal.
    * Encontrar cuál es el rango mayor (`current_max`).
    * **SI** `current_max > max_range`:
        * Guardamos este índice como la caja a dividir (`split_index`).
        * Anotamos qué color ganó (`channel_to_sort` = R, G o B).

3. **SI** no encontramos caja para dividir (`split_index == -1`), **ROMPER** el ciclo.

**B. Ordenar y Cortar**
4. Obtener referencia a la caja ganadora (`box_to_split`).
5. **ORDENAR** (`std::sort`) los píxeles de esa caja usando el color ganador (R, G o B).
6. Calcular la mitad: `mid = start + (end - start) / 2`.
7. **CREAR** nueva caja (`new_box`) que va de `mid` a `end`.
8. **ACTUALIZAR** caja vieja para que termine en `mid`.
9. Agregar `new_box` a la lista `boxes`.
```

#### FASE 3: RECONSTRUCCIÓN (Pintar el promedio)

1.  **Crear Salida:** Crear array de Numpy del mismo tamaño que la imagen original.

2.  **PARA CADA** caja en `boxes`:

    - **Sumar:** Recorrer la caja y sumar R, G y B de todos sus píxeles.
    - **Promediar:** Dividir las sumas entre el tamaño de la caja (`avg_r`, `avg_g`, `avg_b`).
    - **Pintar:** Volver a recorrer los píxeles de la caja:
      - Recuperar `original_index` del píxel.
      - Ir a esa posición en el array de salida (`out_ptr`).
      - Escribir los colores promedio (`avg_r`, `avg_g`, `avg_b`).

3.  **Retornar** imagen resultante.
