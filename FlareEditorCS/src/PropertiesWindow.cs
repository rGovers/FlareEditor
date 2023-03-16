using FlareEngine.Definitions;
using FlareEngine.Maths;
using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Text;
using System.Xml;

namespace FlareEditor
{
    [AttributeUsage(AttributeTargets.Class)]
    public class PWindowAttribute : Attribute
    {
        Type m_overrideType;

        public Type OverrideType
        {
            get
            {
                return m_overrideType;
            }
        }

        public PWindowAttribute(Type a_overrideType)
        {
            m_overrideType = a_overrideType;
        }
    }

    public class PropertiesWindow
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        static extern void WriteDef(string a_path, byte[] a_data);

        void ShowFields(string a_name, ref object a_obj, object a_normVal, Type a_type)
        {
            switch (a_obj)
            {
            case Type _:
            {
                // Ignoring for now as I cannot think of a good way of handling it
                return;
            }
            case IntPtr _:
            {
                // Ignore pointers
                return;
            }
            case bool val:
            {   
                if (GUI.RCheckbox(a_name, ref val, (bool)a_normVal))
                {
                    a_obj = (bool)val;
                }

                break;
            }
            case short val:
            {
                short nVal = (short)a_normVal;
                int sVal = (int)val;
                if (GUI.RIntField(a_name, ref sVal, (int)nVal))
                {
                    a_obj = (short)sVal;
                }

                break;
            }
            case ushort val:
            {
                ushort nVal = (ushort)a_normVal;
                uint sVal = (uint)val;
                if (GUI.RUIntField(a_name, ref sVal, (uint)nVal))
                {
                    a_obj = (ushort)sVal;
                }

                break;
            }
            case int val:
            {
                if (GUI.RIntField(a_name, ref val, (int)a_normVal))
                {
                    a_obj = val;
                }

                break;
            }
            case uint val:
            {
                if (GUI.RUIntField(a_name, ref val, (uint)a_normVal))
                {
                    a_obj = val;
                }

                break;
            }
            case float val:
            {   
                if (GUI.RFloatField(a_name, ref val, (float)a_normVal))
                {
                    a_obj = val;
                }

                break;
            }
            case Vector2 val:
            {
                if (GUI.RVec2Field(a_name, ref val, (Vector2)a_normVal))
                {
                    a_obj = val;
                }

                break;
            }
            case Vector3 val:
            {
                if (GUI.RVec3Field(a_name, ref val, (Vector3)a_normVal))
                {
                    a_obj = val;
                }

                break;
            }
            case Vector4 val: 
            {   
                if (GUI.RVec4Field(a_name, ref val, (Vector4)a_normVal))
                {
                    a_obj = val;
                }

                break;
            }
            case Color val:
            {
                if (GUI.RColorField(a_name, ref val, (Color)a_normVal))
                {
                    a_obj = val;
                }

                break;
            }
            case string val:
            {
                if (GUI.RStringField(a_name, ref val, (string)a_normVal))
                {
                    a_obj = val;
                }

                break;
            }
            default:
            {
                if (a_type == typeof(Def) || a_type.IsSubclassOf(typeof(Def)))
                {
                    Def def = (Def)a_obj;
                    Def normDef = (Def)a_normVal; 

                    if (GUI.RDefField(a_name, ref def, normDef))
                    {
                        a_obj = def;
                    }
                }
                else if (a_type.IsSubclassOf(typeof(Enum)))
                {
                    GUI.REnumField(a_name, ref a_obj, a_normVal);
                }
                else if (a_type.IsArray)
                {
                    Type eType = a_type.GetElementType();

                    Array a = (Array)a_obj;

                    bool add;
                    bool show = GUI.ArrayView(a_name, out add);

                    if (add)
                    {
                        if (a == null)
                        {
                            a = Array.CreateInstance(eType, 1);
                        }
                        else
                        {
                            Array temp = Array.CreateInstance(eType, a.Length + 1);
                            Array.Copy(a, temp, a.Length);
                            a = temp;
                        }
                    }   

                    if (show && a != null)
                    {
                        GUI.Indent();
                        GUI.PushID(a_name);

                        object eNVal = Activator.CreateInstance(eType);

                        int len = a.Length;
                        for (int i = 0; i < len; ++i)
                        {
                            object o = a.GetValue(i);
                            ShowFields($"[{i}]", ref o, eNVal, eType);
                            a.SetValue(o, i);
                        }

                        GUI.PopID();
                        GUI.Unindent();
                    }

                    a_obj = a;
                }
                else if (a_type.IsGenericType && a_type.GetGenericTypeDefinition() == typeof(List<>))
                {
                    Type gType = a_type.GetGenericArguments()[0];
                    MethodInfo method = a_type.GetMethod("Add");
                
                    bool add;
                    bool show = GUI.ArrayView(a_name, out add);
                    
                    object gNVal = Activator.CreateInstance(gType);

                    if (add)
                    {
                        if (a_obj == null)
                        {
                            a_obj = Activator.CreateInstance(a_type);
                        }

                        method.Invoke(a_obj, new object[] { gNVal });
                    }

                    if (show && a_obj != null)
                    {
                        GUI.Indent();
                        GUI.PushID(a_name);

                        object nObj = Activator.CreateInstance(a_type);

                        uint index = 0;
                        IEnumerable enumerable = (IEnumerable)a_obj;
                        foreach (object val in enumerable)
                        {
                            object oVal = val;
                            ShowFields($"[{index++}]", ref oVal, gNVal, gType);
                            method.Invoke(nObj, new object[] { oVal });
                        }

                        a_obj = nObj;

                        GUI.PopID();
                        GUI.Unindent();
                    }
                }
                else
                {
                    if (a_obj == null)
                    {
                        a_obj = Activator.CreateInstance(a_type);
                    }

                    if (GUI.StructView(a_name))
                    {
                        GUI.Indent();
                        GUI.PushID(a_name);

                        FieldInfo[] fields = a_type.GetFields();
                        foreach (FieldInfo field in fields)
                        {
                            if ((field.IsPrivate && field.GetCustomAttributes<SerializableAttribute>() == null) || field.GetCustomAttribute<HideInEditorAttribute>() != null)
                            {
                                continue;
                            }

                            object val = field.GetValue(a_obj);
                            object normObj = Activator.CreateInstance(a_type);

                            ShowFields($"{a_name}.{field.Name}", ref val, field.GetValue(normObj), field.FieldType);

                            field.SetValue(a_obj, val);
                        }

                        GUI.PopID();
                        GUI.Unindent();
                    }
                }

                break;
            }
            }
        }

        static void SerializeField(XmlDocument a_doc, XmlElement a_parent, string a_name, object a_obj, object a_defaultObj, Type a_type)
        {
            switch (a_obj)
            {
            case Type val:
            {
                if (a_defaultObj == null || val != (Type)a_defaultObj)
                {
                    XmlElement element = a_doc.CreateElement(a_name);
                    a_parent.AppendChild(element);
                    element.InnerText = val.ToString();
                }

                break;
            }
            case int val:
            {
                if (a_defaultObj == null || val != (int)a_defaultObj)
                {
                    XmlElement element = a_doc.CreateElement(a_name);
                    a_parent.AppendChild(element);
                    element.InnerText = val.ToString();
                }

                break;
            }
            case uint val:
            {
                if (a_defaultObj == null || val != (uint)a_defaultObj)
                {
                    XmlElement element = a_doc.CreateElement(a_name);
                    a_parent.AppendChild(element);
                    element.InnerText = val.ToString();
                }

                break;
            }
            case float val:
            {
                if (a_defaultObj == null || val != (float)a_defaultObj)
                {
                    XmlElement element = a_doc.CreateElement(a_name);
                    a_parent.AppendChild(element);
                    element.InnerText = val.ToString();
                }

                break;
            }
            default:
            {
                if (a_type == typeof(Def) || a_type.IsSubclassOf(typeof(Def)))
                {
                    Def def = a_obj as Def;
                    Def pDef = a_defaultObj as Def;

                    if (def != null && (pDef == null || def.DefName != pDef.DefName))
                    {
                        XmlElement element = a_doc.CreateElement(a_name);
                        a_parent.AppendChild(element);
                        element.InnerText = def.DefName;
                    }
                }  
                else if (a_type == typeof(string))
                {
                    string val = a_obj as string; 
                    string normVal = a_defaultObj as string;

                    if (val == null)
                    {
                        val = string.Empty;
                    }
                    if (normVal == null)
                    {
                        normVal = string.Empty;
                    }

                    if (val != normVal)
                    {
                        XmlElement element = a_doc.CreateElement(a_name);
                        a_parent.AppendChild(element);
                        element.InnerText = val;
                    }
                }
                else if (a_type.IsGenericType && a_type.GetGenericTypeDefinition() == typeof(List<>) && a_obj != null)
                {
                    Type gType = a_type.GetGenericArguments()[0];
                    
                    PropertyInfo cProperty = a_type.GetProperty("Count");

                    int count = (int)cProperty.GetValue(a_obj);
                    int pCount = 0;
                    if (a_defaultObj != null)
                    {
                        pCount = (int)cProperty.GetValue(a_defaultObj);
                    }

                    if (count > pCount)
                    {
                        XmlElement rootLElement = a_doc.CreateElement(a_name);
                        a_parent.AppendChild(rootLElement);

                        IList l = a_obj as IList;

                        for (int i = pCount; i < count; ++i)
                        {
                            SerializeField(a_doc, rootLElement, "lv", l[i], null, gType);
                        }
                    }
                }
                else if (a_type.IsArray)
                {
                    Type eType = a_type.GetElementType();

                    Array a = a_obj as Array;
                    if (a != null)
                    {   
                        int len = a.Length;
                        if (len > 0)
                        {
                            XmlElement rootAElement = a_doc.CreateElement(a_name);
                            a_parent.AppendChild(rootAElement);

                            for (int i = 0; i < len; ++i)
                            {
                                SerializeField(a_doc, rootAElement, "lv", a.GetValue(i), null, eType);
                            }   
                        }
                    }
                }
                else if (a_type.IsSubclassOf(typeof(Enum)))
                {
                    if (!Enum.Equals(a_obj, a_defaultObj))
                    {
                        XmlElement element = a_doc.CreateElement(a_name);
                        a_parent.AppendChild(element);
                        element.InnerText = a_obj.ToString();
                    }
                }
                else
                {
                    if (a_obj == null || a_obj == a_defaultObj)
                    {
                        return;
                    }

                    XmlElement element = a_doc.CreateElement(a_name);
                    a_parent.AppendChild(element);

                    FieldInfo[] fields = a_type.GetFields();
                    if (a_defaultObj != null)
                    {
                        foreach (FieldInfo field in fields)
                        {
                            if (field.GetCustomAttribute<NonSerializedAttribute>() != null)
                            {
                                continue;
                            }

                            SerializeField(a_doc, element, field.Name, field.GetValue(a_obj), field.GetValue(a_defaultObj), field.FieldType);
                        }
                    }
                    else
                    {
                        foreach (FieldInfo field in fields)
                        {
                            if (field.GetCustomAttribute<NonSerializedAttribute>() != null)
                            {
                                continue;
                            }

                            SerializeField(a_doc, element, field.Name, field.GetValue(a_obj), null, field.FieldType);
                        }
                    }
                }

                break;
            }
            }
        }

        static void SerializeDefs(string[] a_paths)
        {
            uint defCount = (uint)a_paths.LongLength;
            List<Def> defs = DefLibrary.GetDefs();

            for (uint i = 0; i < defCount; ++i)
            {
                string path = a_paths[i];

                Def mainDef = null;
                foreach (Def def in defs)
                {
                    if (def.DefPath == path)
                    {
                        mainDef = def;

                        break;
                    }
                }

                if (mainDef == null)
                {
                    continue;
                }

                Type type = mainDef.GetType();

                XmlDocument doc = new XmlDocument();
                doc.AppendChild(doc.CreateXmlDeclaration("1.0", "utf-16", null));

                XmlElement root = doc.CreateElement(mainDef.GetType().ToString());
                doc.AppendChild(root);
                root.SetAttribute("Name", mainDef.DefName);
                
                Def parentDef = (Def)Activator.CreateInstance(type);
                if (!string.IsNullOrWhiteSpace(mainDef.DefParentName))
                {
                    parentDef = DefLibrary.GetDef(mainDef.DefParentName);
                    root.SetAttribute("Parent", mainDef.DefParentName);
                }

                Type pType = parentDef.GetType();

                FieldInfo[] fields = type.GetFields();
                foreach (FieldInfo field in fields)
                {
                    if (field.GetCustomAttribute<NonSerializedAttribute>() != null)
                    {
                        continue;
                    }
                    
                    Type fType = field.FieldType;

                    object normVal = null;
                    if (pType.GetField(field.Name) != null)
                    {
                        normVal = field.GetValue(parentDef);
                    }

                    SerializeField(doc, root, field.Name, field.GetValue(mainDef), normVal, fType);
                }

                MemoryStream stream = new MemoryStream();
                XmlTextWriter writer = new XmlTextWriter(stream, Encoding.Unicode);
                writer.Formatting = Formatting.Indented;

                doc.WriteContentTo(writer);
                writer.Flush();
                stream.Flush();

                stream.Position = 0;

                StreamReader reader = new StreamReader(stream);
                byte[] bytes = Encoding.Unicode.GetBytes(reader.ReadToEnd());

                WriteDef(path, bytes);
            }
        }

        public virtual void OnGUI(object a_object)
        {
            Type objType = a_object.GetType();

            if (objType.IsPrimitive || objType == typeof(decimal) || objType == typeof(float) || objType == typeof(double))
            {
                return;
            }

            object normObj = Activator.CreateInstance(objType);

            FieldInfo[] fields = objType.GetFields();
            foreach (FieldInfo field in fields)
            {
                string fName = field.Name;

                if ((field.IsPrivate && field.GetCustomAttributes<SerializableAttribute>() == null) || field.GetCustomAttribute<HideInEditorAttribute>() != null)
                {
                    continue;
                }

                object val = field.GetValue(a_object);

                ShowFields(field.Name, ref val, field.GetValue(normObj), field.FieldType);
 
                field.SetValue(a_object, val);

                EditorTooltipAttribute tooltip = field.GetCustomAttribute<EditorTooltipAttribute>();
                if (tooltip != null)
                {
                    GUI.Tooltip(fName, tooltip.Tooltip);
                }
            }
        }
    }
}
