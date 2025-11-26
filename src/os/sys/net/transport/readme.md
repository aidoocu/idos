# Capa de Transporte

## TCP

## UDP
El protocolo de datagramas de usuario, abreviado como UDP (User Datagram Protocol), es un protocolo que permite la transmisión **sin conexión** de datagramas en redes basadas en IP en funciona en la capa de transporte. Fue especificado en 1980 en la [RFC768](https://tools.ietf.org/html/rfc768).

Un segmento UDP consiste en una cabecera de 8 bytes seguida de los datos como se muestra a continuación.

                  0      7 8     15 16    23 24    31
                 +--------+--------+--------+--------+
                 |     Source      |   Destination   |
                 |      Port       |      Port       |
                 +--------+--------+--------+--------+
                 |                 |                 |
                 |     Length      |    Checksum     |
                 +--------+--------+--------+--------+
                 |
                 |          data octets ...
                 +---------------- ...

- **Puerto de origen:** es el número de puerto relacionado con la aplicación del remitente del segmento UDP. Este campo representa una dirección de respuesta para el destinatario. Por lo tanto, este campo es opcional. Esto significa que si el puerto de origen no está especificado, los 16 bits de este campo se pondrán en cero. En este caso, el destinatario no podrá responder (lo cual no es estrictamente necesario, en particular para mensajes unidireccionales).
- **Puerto de destino:** este campo contiene el puerto correspondiente a la aplicación del equipo receptor al que se envía.
- **Longitud:** este campo especifica la longitud total del segmento, con el encabezado incluido. Sin embargo, el encabezado tiene una longitud de 4 x 16 bits (que es 8 x 8 bits), por lo tanto la longitud del campo es necesariamente superior o igual a 8 bytes.
- **Suma de comprobación:** es una suma de comprobación realizada de manera tal que permita controlar la integridad del segmento. La suma de comprobación UDP incluye la misma pseudocabecera de formato, la cabecera UDP, y los datos, rellenados con una cantidad par de bytes de ser necesario.

 