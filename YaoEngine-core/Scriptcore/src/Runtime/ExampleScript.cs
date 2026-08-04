namespace YaoEngine
{
    public sealed class ExampleScript : ScriptBehaviour
    {
        private bool m_HasLoggedFirstUpdate;

        public override void OnCreate()
        {
            Log("ExampleScript.OnCreate called from Mono");
        }

        public override void OnUpdate(float timestep)
        {
            if (m_HasLoggedFirstUpdate)
                return;

            m_HasLoggedFirstUpdate = true;
            Log($"ExampleScript.OnUpdate first frame timestep={timestep:0.000}");
        }
    }
}
