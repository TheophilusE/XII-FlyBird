import xii = require("TypeScript/xii");

// export class Scorepoint extends xii.TypescriptComponent {
export class Scorepoint extends xii.TickedTypescriptComponent {
  /* BEGIN AUTO-GENERATED: VARIABLES */
    /* END AUTO-GENERATED: VARIABLES */

  constructor() {
    super();
  }

  static RegisterMessageHandlers() {
    // You can only call "RegisterMessageHandler" from within this function
    xii.TypescriptComponent.RegisterMessageHandler(
      xii.MsgSetColor,
      "OnMsgSetColor"
    );

    xii.TypescriptComponent.RegisterMessageHandler(
      xii.MsgTriggerTriggered,
      "OnTriggerActivated"
    );
  }

  // Initialize(): void { }
  // Deinitialize(): void { }
  // OnActivated(): void { }
  // OnDeactivated(): void { }

  OnSimulationStarted(): void {
    this.SetTickInterval(xii.Time.Milliseconds(1000));
  }

  OnTriggerActivated(msg: xii.MsgTriggerTriggered): void {
    if (msg.TriggerState == xii.TriggerState.Activated) {
      // xii.Log.Info("Triggered from TypeScript!");

      // Update Player Score
      let currentScore: number = xii.Debug.ReadCVar_Int("FlyBird.PlayerScore") + 1;
      xii.Debug.WriteCVar_Int("FlyBird.PlayerScore", currentScore);
    }
  }

  OnMsgSetColor(msg: xii.MsgSetColor): void {
    xii.Log.Info(
      "MsgSetColor: " +
        msg.Color.r +
        ", " +
        msg.Color.g +
        ", " +
        msg.Color.b +
        ", " +
        msg.Color.a
    );
  }

  Tick(): void {
    // if a regular tick is not needed, remove this and derive directly from xii.TypescriptComponent
    // xii.Log.Info("Scorepoint.Tick()")
  }
}
