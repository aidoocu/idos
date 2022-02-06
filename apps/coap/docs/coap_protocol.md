# CoAP
CoAP o Constrained Application Protocol es un protocolo de transferencia Web para uso en ambientes restringidos. Está definido en la [RFC7252](https://datatracker.ietf.org/doc/html/rfc7252) de 2014. La mayor parte de este documento es una tradución comentada de la [RFC7252](https://datatracker.ietf.org/doc/html/rfc7252) y otras en torno a CoAP que han sido tenidas en cuenta para la construcción de esta implentación. Está enriquecida además con comentarios y criterios de implementación. 

Está pensado para trabajar sobre UDP pues contiene un mecanismo para la confirmación de entrega.

## Encabezado
A continuación se muestra el encabezado de CoAP que consta de apenas 4 bites fijos.

         0                   1                   2                   3
         0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
        +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        |Ver| T |  TKL  |      Code     |          Message ID           |
        +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        |   Token (if any, TKL bytes) ...
        +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        |   Options (if any) ...
        +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        |1 1 1 1 1 1 1 1|    Payload (if any) ...
        +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

- **Version (Ver)** Define la versión del protocolo.
- **Type (T)** El tipo de mensaje, puede tomar cuatro valores: Confimable, Non-confirmable, Acknowledgement y Reset.
- **Token Length (TKL)** longitud del Token, los bytes 9-15 están reservados y no deben usarse.
- **Code** Entero sin signo de 8 bits, dividido en una clase de 3 bits (bits más significativos) y un detalle de 5 bits (bits menos significativos), documentado como "c.dd" donde "c" es un dígito de 0 a 7 para el subcampo de 3 bits y "dd" son dos dígitos de 00 a 31 para el subcampo de 5 bits. de 5 bits. La clase puede indicar una solicitud (0), una respuesta éxito (2), una respuesta de error del cliente (4) o una respuesta de error del del servidor (5).  (Todos los demás valores de clase están reservados). Caso especial, el código 0.00 indica un mensaje vacío.  En el caso de una solicitud, el campo código indica el Método de solicitud; en el caso de una respuesta, un código de respuesta. 
- **Message ID** Permite evitar duplicidad y facilitar mensajes del tipo Acknowledgement y Reset.
- **Payload** Carga útil del mensaje.

# Opciones del CoAP
Las opciones contenidas en un paquete CoAP se encuentran a continuación de la cabecera. Estas opciones deben estar ordenadas mediante su Option Number, los Option Number impares corresponden a opciones críticas y los pares a opcionales. Nótese que en ningún caso se habla de opciones obligatorias, la diferencia entre críticas y opcionales viene dada en cómo se trata la situación que se da cuando se recibe una opción no reconocida.

Cada opción tendrá esta forma:

                  0   1   2   3   4   5   6   7
                +---------------+---------------+
                |  Option Delta | Option Length |   1 byte
                +---------------+---------------+
                /         Option Delta          /   0-2 bytes
                \          (extended)           \
                +-------------------------------+
                /         Option Length         /   0-2 bytes
                \          (extended)           \
                +-------------------------------+
                \                               \
                /         Option Value          /   0 or more bytes
                \                               \
                +-------------------------------+

El campo Option Delta incluye el número de opción desplazado. El número de opción identifica como debe ser interpretada la opción. A continuación se muestra la tabla de números de opción.


                                CoAp Options
        +-----+---+---+---+---+----------------+--------+--------+----------+
        | No. | C | U | N | R | Name           | Format | Length | Default  |
        +-----+---+---+---+---+----------------+--------+--------+----------+
        |   1 | x |   |   | x | If-Match       | opaque | 0-8    | (none)   |
        |   3 | x | x | - |   | Uri-Host       | string | 1-255  | (see     |
        |     |   |   |   |   |                |        |        | below)   |
        |   4 |   |   |   | x | ETag           | opaque | 1-8    | (none)   |
        |   5 | x |   |   |   | If-None-Match  | empty  | 0      | (none)   |
        |   7 | x | x | - |   | Uri-Port       | uint   | 0-2    | (see     |
        |     |   |   |   |   |                |        |        | below)   |
        |   8 |   |   |   | x | Location-Path  | string | 0-255  | (none)   |
        |  11 | x | x | - | x | Uri-Path       | string | 0-255  | (none)   |
        |  12 |   |   |   |   | Content-Format | uint   | 0-2    | (none)   |
        |  14 |   | x | - |   | Max-Age        | uint   | 0-4    | 60       |
        |  15 | x | x | - | x | Uri-Query      | string | 0-255  | (none)   |
        |  17 | x |   |   |   | Accept         | uint   | 0-2    | (none)   |
        |  20 |   |   |   | x | Location-Query | string | 0-255  | (none)   |
        |  35 | x | x | - |   | Proxy-Uri      | string | 1-1034 | (none)   |
        |  39 | x | x | - |   | Proxy-Scheme   | string | 1-255  | (none)   |
        |  60 |   |   | x |   | Size1          | uint   | 0-4    | (none)   |
        +-----+---+---+---+---+----------------+--------+--------+----------+
                C=Critical, U=Unsafe, N=NoCacheKey, R=Repeatable


Para calcular el Option Delta entonces involucramos el número de opción más el desplazamiento. Veamos un ejemplo.
Analicemos el mensaje siguiente. Tenga en cuenta que el mensaje está en hex, y en la explicación los vemos en dec.

        48 01 73 41 a4 8c 18 67 09 19 39 66 b5 68 65 6c 6c 6f 11 00

        48 : 01 (ver 01), 00 (CON), 8 (token len)
        01 : (code GET) 
        73 41 : (message ID)
        a4 8c 18 67 09 19 39 66 : (token)
        b5 : b (option number = 0(delta) + b(option delta) = 11(dec)), 5 (option len)
             option number = 11 (b hex) significa que la opción es tipo Uri-Path, un string (len 5)
        68 65 6c 6c 6f : option (hello)
        11 : 1 (option number = b(delta) + 1(option delta) = 12(dec))
             option number = 12 (c hex) significa que la opción es tipo Content-Format, uint (len 1)
        00 : option (text/plain)

## Metodos
CoAP contempla los métodos básicos de http GET, POST, PUT y DELETE. No obstante idOS está pensado para dispositivo muy restringidos en los cuales los recursos se crean como una estructura al arranque del programa y viven en memoria todo el tiempo. En este sentido no debería tener sentido (y lo asumiremos así) que un extremo solicite crear o eliminar un recurso, por lo que no implementaremos POST y DELETE. No obstante dejamos comentada en la estructura del recurso los callbacks para estos métodos, usted es libre de implementarlos.
### GET
El método GET recupera una representación de la información que actualmente corresponde al recurso identificado por el URI de la solicitud. Si la solicitud incluye una opción Accept, ésta indica el formato de contenido preferido para la respuesta. IdOS solo acepta **text/plain** por ahora. 
Si la solicitud incluye una opción  ETag Option, el método GET solicita que se valide el ETag y que la representación se transfiera sólo si la validación falla.  
Si GET tiene éxito (aunque la RFC7252 no obliga) idOS responde con 2.05 (Content) o 2.03 (Valid). 
El método GET es seguro e idempotente.
### PUT
El método PUT solicita que el recurso identificado por la solicitud
sea actualizado o creado con la representación adjunta. IdOS no incluye la creación de nuevos recursos. Si la semántica de la petición implica un crear un recurso idOS responderá con 4.03 (Forbidden).
El formato de representación de formato de representación se especifica por el tipo de medio y la codificación indicados en la opción Content-Format, si se proporciona. IdOS solo acepta **text/plain** por ahora.
Si existe un recurso en el URI de la solicitud, la representación adjunta debería considerarse una versión modificada de ese recurso, y un código de respuesta 2.04 (Changed) es devuelto. 
Si no existe ningún recurso, según la RFC se debería poder crear uno nuevo con ese URI, sin embargo idOS no lo implementa, así que responderá con 4.03 (Forbidden).
PUT no es seguro pero es idempotente.
### POST y DELETE
Estos métodos no está desarrollados para idOS hasta ahora por las razones ya plantadas. No obstante usted es libre de implementarlos.
