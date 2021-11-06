#modulos
import serial
#from serial.serialutil import EIGHTBITS, PARITY_NONE, STOPBITS_ONE, Timeout
#abrir puerto
ser = serial.Serial(port='COM3', 
                    baudrate=115200, 
                    bytesize=serial.EIGHTBITS, 
                    parity=serial.PARITY_NONE, 
                    stopbits=serial.STOPBITS_ONE)

#ser=serial.serial_for_url('loop://',timeout=1)

#INTERFACE
sep= "//" + "-"*75 + "//\n"
print(sep)
print('\t\tPROYECTO FINAL ED3\n')
print('Escriba un comando para seguir, se pueden consultar los que estan\ndisponibles escribiendo "cmd".\n')
print(sep)

#bandera de finalizacion
exit_flag=False

#mientras se requiera un comando
while exit_flag == 0:
    #linpiar buffers
    ser.flushInput
    ser.flushOutput

    #ingrasar comando
    cmd = input("Ingrese un comando >> ").strip().lower()
    
    #si cmd, imprimir comandos
    if cmd=='cmd':
        print('\nComandos disponibles:\n')
        print('\t* "exit"    -> cerrar el programa\n')
        print('\t* "onradio" -> iniciar transmision\n')
        print('\t* "getkey"  -> obtener clave de validacion actual del receptor\n')
        print('\t* "setkey"  -> modificar clave de validacion del receptor\n')
    
    #si exit, finalizar
    elif cmd=='exit':
        exit_flag=True
    
    #si onradio, activar transmision
    elif cmd=='onradio':
        #enviar comando al micro, es necesario pasarlo a bytes con encode
        ser.write("o\n".encode())

    #si offradio, finalizar transmision
    elif cmd=='offradio':
        ser.write("f\n".encode())
    
    #si getkey, pedir al micro la llave actual
    elif cmd=='getkey':
        #pedir
        ser.write("g\n".encode())
        #escuchar 1 byte
        currentKey=ser.read(1)
        print(f"La clave actual es: 0x{currentKey.hex()}\n")
    
    #si setkey, reconfigurar la llave del micro
    elif cmd=='setkey':
        newKey=input('Ingrese la nueva clave de validación (1 Byte en HEX) >> ')
        
        #si fuera de rango, error
        if int(newKey,16) > (2**8)-1:
            print('Error, la clave no es valida\n')
        #sino, cambiar llave
        else:
            ser.write('s'.encode())
            ser.write(bytes.fromhex(newKey))
            #esperar byte de confirmacion
            readyToLoad=ser.read(1)
    
            if readyToLoad:
                #pasar a bytes y mandar la nueva llave
                print("Clave cargada con exito.\n")
            else:
                print('Ocurrio un error cargando la nueva clave\n')
    
    #si no existe comando
    else:
        print('\t*COMANDO INVALIDO*\n')

#cerrar puerto
ser.close()