# Protocol stack

The protocol stack is an example of how the Max22x88 drivers can be used in an application.

This stack implements a communication protocol that introduces the concept of device addresses and operation codes. This allows the development of multidrop applications with unicast addressing. The protocol itself transparent to the user of the stack.

## The protocol

Each packet consists of a 4-byte header and up to a 255-byte long payload.

The header is:
Self address - Destination address - Operation code - Data length

and it's followed by 0 up to 255 data bytes, as specified in the Data length field.

- Self address / Destination address: The user defines the address of each node in the network.
- Operation code: A field with an user-defined value that can be processed by user application callbacks.

## Integration with drivers

The protocol stack integrates with the drivers in two ways: for outbound and for inbound data.

For outbound data, a Tx callback has to be registered by passing a callback and context arguments to `tx_cb` and `tx_cb_state` parameters in the `adi_hbs_Init` function. Whenever the `adi_hbs_Send` is function, the registered callback will be called with the context arguments. See the files in `examples/two_nodes/stack/integration/max22x88` for the integration provided for the Max22x88 drivers. Note that it refers only to the outbound data integration.

For inbound data, an Rx callback has to be registered by calling `adi_hbs_RegisterRxCb`. Once it's registered, the user application passes any incoming data to the `adi_hbs_Received` function. When a packet is received, the registered callback will be called with the packet content. Note: `adi_hbs_Received` is intended to be called from the main application and it may not be suitable to call it from an interrupt context.
