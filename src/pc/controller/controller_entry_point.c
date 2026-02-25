#include "macros.h"

#include "lib/src/libultra_internal.h"
#include "lib/src/osContInternal.h"

#include "controller_dc.h"

// Global variable defined in controller_dc.c
extern uint8_t gControllerBits;

static struct ControllerAPI *controller_implementations[] = {
    &controller_dc,
};

s32 osContInit(UNUSED OSMesgQueue *mq, u8 *controllerBits, UNUSED OSContStatus *status) {
    size_t i;

    for (i = 0; i < sizeof(controller_implementations) / sizeof(struct ControllerAPI *); i++) {
        controller_implementations[i]->init();
    }
   // 2. IMPORTANT: Invoke an initial read to detect if a remote control is present at startup.
    // This ensures the message appears from the very first second.
    OSContPad temp_pad;
    controller_dc.read(&temp_pad);

    // 3. Report the actual bits to the SM64 engine
    *controllerBits = gControllerBits; 
    return 0;
}

s32 osContStartReadData(UNUSED OSMesgQueue *mesg) {
    return 0;
}

void osContGetReadData(OSContPad *pad) {
    size_t i;

    pad->button = 0;
    pad->stick_x = 0;
    pad->stick_y = 0;
    pad->errnum = 0;

    for (i = 0; i < sizeof(controller_implementations) / sizeof(struct ControllerAPI *); i++) {
        controller_implementations[i]->read(pad);
    }

    // 3. Lógica de Error Dinámica:
    // Si gControllerBits es 0 (mando desconectado), activamos el flag de error
    // que el motor de SM64 usa para mostrar el mensaje de pausa.
    if (gControllerBits == 0) {
        pad->errnum = 255; // Código de error estándar de Ultra64 para "No Controller"
    } else {
        pad->errnum = 0;   // Todo está bien
    }
}
