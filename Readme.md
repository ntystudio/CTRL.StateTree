# Extended State Tree

This plugin adds a range of useful new state tree utility tasks and property functions.

## Tasks

### Events

#### Trigger Gameplay Event

Trigger a GameplayEvent on target actor(s), via `AbilitySystemComponent`. Fire on a single actor, or an array of actors, both of which can be bound to data.
Can fire different (or no) events on Enter and Exit state.

#### Forward Gameplay Event to StateTree Event

Any GameplayEvents received on the target actor, matching the specified tag, will be re-emitted as StateTree events. Allows using normal state tree transitions to respond to Gameplay Events e.g. on `Actor.Died` event transition to `Dead` state.
Useful as a global task, bound to the tree owner. Combined with "Trigger GameplayEvent" this allows bidirectional communication between the state tree and actors.
Options for `bOnlyTriggerOnce` and `bOnlyMatchExact`, like the `WaitGameplayEvent` Blueprint node.

### Input

#### Change Input Config

Change input config (e.g. Game Only, Game & UI, UI Only) & associated options (e.g. mouse capture, cursor, input flush) on target player controller. Reverts on exit.
Changes are debounced and applied on next tick to prevent flickering.
Uses a stack of configs, so when you exit a state, the next config in the stack will be used.
You may want to put a default config in your root state to prevent flickering.

### UI

#### ActivateWidget

Activates/deactivates a target `UCommonActivatableWidget`. Optionally performs the inverse on exit.

### Components

#### Set Component Visibility

Sets the visibility of the target component to the target value. Unsets the visibility on exit.

#### Set Component Visibility In-Game

Same as "Set Component Visibility" but calls the appropriate `SetHiddenInGame` on the target component.

### Debug

#### Print Text

Log a message to the output console and/or screen on enter/exit state. Same general options as regular `Print Text` Blueprint Node.

## Property Functions

### String to Text
Useful for binding text properties to string data.

### Bool To String
Useful for logging.

* Random Float
* Random Integer
* Object Is Valid

## License

MIT