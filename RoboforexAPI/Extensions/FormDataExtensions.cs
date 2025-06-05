using System.Reflection;
using System.Text.Json.Serialization;

namespace RoboforexAPI.Extensions
{
    public static class FormDataExtensions
    {
        public static Dictionary<string, string> ToFormData<T>(this T obj)
        {
            if (obj == null) throw new ArgumentNullException(nameof(obj));

            Dictionary<string, string> formData = new Dictionary<string, string>();
            var t = typeof(T);
            PropertyInfo[] properties = t.GetProperties();

            foreach (var property in properties)
            {
                // Get the JsonPropertyName attribute, if it exists
                var jsonPropertyName = property.GetCustomAttribute<JsonPropertyNameAttribute>();
                if (jsonPropertyName != null)
                {
                    // Get the property value
                    var value = property.GetValue(obj);
                    // Convert the value to string, handling nulls and using invariant culture for numbers
                    string? stringValue = value switch
                    {
                        null => string.Empty,
                        string str => str,
                        double d => d.ToString(System.Globalization.CultureInfo.InvariantCulture),
                        float f => f.ToString(System.Globalization.CultureInfo.InvariantCulture),
                        decimal dec => dec.ToString(System.Globalization.CultureInfo.InvariantCulture),
                        _ => value.ToString()
                    };
                    if (!string.IsNullOrEmpty(stringValue))
                    {
                        formData.Add(jsonPropertyName.Name, stringValue);
                    }
                }
            }

            return formData;
        }
    }
}
