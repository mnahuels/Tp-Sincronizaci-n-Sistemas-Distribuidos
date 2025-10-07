# 🖥️ Trabajo Práctico: Sincronización de Relojes en Sistemas Distribuidos

Este proyecto fue desarrollado como parte de la asignatura **Sistemas Distribuidos**, y tiene como objetivo **implementar y comparar distintos algoritmos clásicos de sincronización de relojes** en un entorno simulado con **C++**.

---

## 📋 Consigna

> A. Un Sistema Distribuido funciona en 4 Computadoras (1, 2, 3, 4).  
> Cada computadora ejecuta un proceso `P`.  
> Suponiendo que la aplicación debe ejecutarse en el orden `P1, P2, P3, P4`,  
> realizar en C++ los programas para sincronizar las máquinas utilizando el **Algoritmo de Lamport**.  
>
> B. Realizar programas en C++ para sincronizar computadoras utilizando el **Algoritmo de Berkeley**.  
>
> C. Programar el **método de Cristian** para sincronizar relojes.

---

## 🧠 Objetivo

Comprender los fundamentos de la **sincronización de relojes en sistemas distribuidos**, tanto en:
- **Tiempo lógico** (Lamport),
- **Tiempo físico promedio** (Berkeley),
- **Cliente-servidor** (Cristian).

---

## ⚙️ Estructura del Proyecto

| Archivo | Descripción |
|----------|--------------|
| `lamport.cpp` | Implementa el algoritmo de **Lamport** con 4 procesos y mensajes simulados mediante hilos. |
| `berkeley.cpp` | Simula la sincronización **Berkeley**, donde un proceso maestro promedia las diferencias de reloj. |
| `cristian.cpp` | Implementa el método **Cristian**, con un servidor de tiempo y clientes que estiman su hora real. |

---

👥 Team Members  
- Santiago Alloud  
- Felipe Palazzi  
- Mauricio Nahuel Salto 

---
