namespace YaoEngine
{
    public abstract class ScriptBehaviour
    {
        public virtual void OnCreate()
        {
        }

        public virtual void OnUpdate(float timestep)
        {
        }

        protected void Log(string message)
        {
            InternalCalls.NativeLog(message);
        }
    }
}
