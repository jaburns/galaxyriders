using System;
using System.Collections.Generic;
using System.Xml;
using UnityEditor;
using UnityEngine;

public class StarlingAtlasSlicer : EditorWindow
{
    [MenuItem("CONTEXT/TextureImporter/Slice From XML")]
    static public void SliceUsingXML(MenuCommand command)
    {
        var importer = command.context as TextureImporter;
        var xmlAsset = AssetDatabase.LoadMainAssetAtPath(getXMLPath(importer.assetPath)) as TextAsset;
        var document = new XmlDocument();
        document.LoadXml(xmlAsset.text);

        var root = document.DocumentElement;

        if (root.Name != "TextureAtlas") {
            Debug.LogError("XML needs to have a 'TextureAtlas' root node!");
            return;
        }

        var texture = AssetDatabase.LoadMainAssetAtPath(importer.assetPath) as Texture2D;
        var textureHeight = texture.height;
        var metaDataList = new List<SpriteMetaData>();

        float pivotX = 0.0f;
        float pivotY = 0.0f;

        foreach (XmlNode childNode in root.ChildNodes) {
            if (childNode.Name == "SubTexture") {
                try {
                    if (childNode.Attributes["pivotX"] != null) {
                        pivotX = Convert.ToSingle(childNode.Attributes["pivotX"].Value);
                    }
                    if (childNode.Attributes["pivotY"] != null) {
                        pivotY = Convert.ToSingle(childNode.Attributes["pivotY"].Value);
                    }

                    int width = Convert.ToInt32(childNode.Attributes["width"].Value);
                    int height = Convert.ToInt32(childNode.Attributes["height"].Value);
                    int x = Convert.ToInt32(childNode.Attributes["x"].Value);
                    int y = textureHeight - height - Convert.ToInt32(childNode.Attributes["y"].Value);
                    float frameX = childNode.Attributes["frameX"] != null ? Convert.ToSingle(childNode.Attributes["frameX"].Value) : 0;
                    float frameY = childNode.Attributes["frameY"] != null ? Convert.ToSingle(childNode.Attributes["frameY"].Value) : 0;

                    float px = (frameX + pivotX) / (float)width;
                    float py = 1.0f - (frameY + pivotY) / (float)height;

                    var spriteMetaData = new SpriteMetaData
                    {
                        alignment = (int)SpriteAlignment.Custom,
                        border = new Vector4(),
                        name = childNode.Attributes["name"].Value,
                        pivot = new Vector2(px, py),
                        rect = new Rect(x, y, width, height)
                    };

                    metaDataList.Add(spriteMetaData);
                }
                catch (Exception exception) {
                    Debug.LogException(exception);
                    return;
                }
            }
        }

        importer.spriteImportMode = SpriteImportMode.Multiple;
        importer.spritesheet = metaDataList.ToArray();
        EditorUtility.SetDirty(importer);

        try {
            AssetDatabase.StartAssetEditing();
            AssetDatabase.ImportAsset(importer.assetPath);
        } finally {
            AssetDatabase.StopAssetEditing();
        }
    }

    static private string getXMLPath(string pngPath)
    {
        var dotIndex = pngPath.LastIndexOf('.');
        return pngPath.Substring(0, dotIndex) + ".xml";
    }
}
