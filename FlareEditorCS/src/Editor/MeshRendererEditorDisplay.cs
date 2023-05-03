using FlareEngine;
using FlareEngine.Definitions;
using FlareEngine.Maths;
using FlareEngine.Rendering;

namespace FlareEditor.Editor
{
    [EDisplay(typeof(MeshRenderer))]
    public class MeshRendererEditorDisplay : EditorDisplay
    {
        public override void Render(bool a_selected, Def a_component, Matrix4 a_transform)
        {
            MeshRendererDef def = a_component as MeshRendererDef;
            if (def == null)
            {
                return;
            }

            Model mdl = AssetLibrary.LoadModel(def.ModelPath);
            if (mdl == null)
            {
                return;
            }
            Material mat = AssetLibrary.GetMaterial(def.MaterialDef);
            if (mat == null)
            {
                return;
            }

            RenderCommand.BindMaterial(mat);
            RenderCommand.DrawModel(a_transform, mdl);
        }
    }
}