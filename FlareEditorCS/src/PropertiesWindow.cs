using FlareEngine.Definitions;
using FlareEngine.Maths;
using System;
using System.Collections.Generic;
using System.Reflection;

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
        public virtual void OnGUI(object a_object)
        {
            Type objType = a_object.GetType();

            if (objType.IsPrimitive || objType == typeof(decimal) || objType == typeof(float) || objType == typeof(double))
            {
                return;
            }

            bool isDef = objType.IsSubclassOf(typeof(Def));

            FieldInfo[] fields = objType.GetFields();
            foreach (FieldInfo field in fields)
            {
                if (isDef)
                {
                    switch (field.Name)
                    {
                    case "DefName":
                    case "DefPath":
                    {
                        continue;
                    }
                    }
                }

                if ((field.IsPrivate && field.GetCustomAttributes<SerializableAttribute>() == null) || field.GetCustomAttribute<NonSerializedAttribute>() != null)
                {
                    continue;
                }

                switch (field.GetValue(a_object))
                {
                case Type _:
                {
                    // Ignoring for now as I cannot think of a good way of handling it
                    break;
                }
                case int val:
                {
                    if (GUI.IntField(field.Name, ref val))
                    {
                        field.SetValue(a_object, val);
                    }

                    break;
                }
                case uint val:
                {
                    if (GUI.UIntField(field.Name, ref val))
                    {
                        field.SetValue(a_object, val);
                    }   

                    break;
                }
                case float val:
                {   
                    if (GUI.FloatField(field.Name, ref val))
                    {
                        field.SetValue(a_object, val);
                    }

                    break;
                }
                case Vector2 val:
                {
                    if (GUI.Vec2Field(field.Name, ref val))
                    {
                        field.SetValue(a_object, val);
                    }

                    break;
                }
                case Vector3 val:
                {
                    if (GUI.Vec3Field(field.Name, ref val))
                    {
                        field.SetValue(a_object, val);
                    }

                    break;
                }
                case Vector4 val: 
                {   
                    if (GUI.Vec4Field(field.Name, ref val))
                    {
                        field.SetValue(a_object, val);
                    }

                    break;
                }
                case string val:
                {
                    if (GUI.StringField(field.Name, ref val))
                    {
                        field.SetValue(a_object, val);
                    }

                    break;
                }
                case object val:
                {
                    Type type = val.GetType();
                    
                    if (type.IsSubclassOf(typeof(Def)))
                    {

                    }
                    else if (type.IsSubclassOf(typeof(Enum)))
                    {
                        if (GUI.EnumField(field.Name, ref val))
                        {
                            field.SetValue(a_object, val);
                        }
                    }
                    else if (type.IsGenericType && type.GetGenericTypeDefinition() == typeof(List<>))
                    {
                        
                    }
                    else
                    {
                        if (val == null)
                        {
                            val = Activator.CreateInstance(type);
                        }

                        OnGUI(val);
                        
                        field.SetValue(a_object, val);
                    }

                    break;
                }
                }
            }
        }
    }
}
