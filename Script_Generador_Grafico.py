import numpy as np
import matplotlib.pyplot as plt

# Carga los datos desde el archivo .dat
archivo = 'C:\\Users\\Rog Itx\\Downloads\\TDC\\data.dat'
datos = np.loadtxt(archivo)

indices = np.arange(len(datos))

# Creamos el gráfico
plt.figure(figsize=(10, 5))
plt.plot(indices, datos, label='Angulo')

plt.xlabel('Ciclo')
plt.ylabel('Angulo')
plt.title('Gráfico de Salidas del Sistema')
plt.legend()
plt.grid(True)

plt.show()