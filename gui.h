#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct GuiRectangle{
  int x;
  int y;
  int w;
  int h;
} GuiRectangle;

typedef struct GuiElement{
  GuiRectangle r; //position and dims (origin and dims if relative)
  int border; //border size relative
  char type; //type (b-button or f-field)
  char* text; //text contents
  Color color; //primary color
  Color borderColor;
  size_t name;
  Color textColor;
} GuiElement;

typedef struct GuiElementArrayHeader{
  int count;
  int size;
  char type;
  int vertical;
  int horizontal;
}GuiElementArrayHeader;


GuiElement* InitGuiElementArray(char type, int vertical, int horizontal);

GuiElementArrayHeader* GetHead(GuiElement* array);

GuiElement* PushGuiElement(GuiElement ge, GuiElement* array);

GuiElement* InitGuiFromEnum(char type, int numOfElements, int vertical, int horizontal, int BorderSize);

void GuiDrawTextInBox(char* text, int originX, int originY, int width, int height, Color TextColor);

void DrawElement(GuiElement ge, GuiElementArrayHeader head);

#if defined(GUI_IMPLEMENTATION)

GuiElement* InitGuiElementArray(char type, int vertical, int horizontal)
{
  GuiElement* arrayPointer = malloc(sizeof(GuiElementArrayHeader)+sizeof(GuiElement)*64);
  arrayPointer = (GuiElement*)((GuiElementArrayHeader*)arrayPointer+1); 
  *(GetHead(arrayPointer)) = (GuiElementArrayHeader){0, 64, type, vertical, horizontal};
  
  return arrayPointer;
}

GuiElementArrayHeader* GetHead(GuiElement* array)
{
  return (GuiElementArrayHeader*)array-1;
}

GuiElement* PushGuiElement(GuiElement ge, GuiElement* array)
{
  if(GetHead(array)->size < GetHead(array)->count+1)
  {
    array = realloc(GetHead(array), sizeof(GuiElementArrayHeader) + sizeof(GuiElement)*(GetHead(array)->count+2));
    if(array!=NULL)
      array = (GuiElement*)((GuiElementArrayHeader*)array+1);
    else
    {
      printf("Could not realloc memory for push operation %s, %d", __FILE__, __LINE__);
      exit(EXIT_FAILURE);
    }
    GetHead(array)->size+=2;
  }
  array[GetHead(array)->count] = ge;
  GetHead(array)->count+=1;
  
  return array;

}

GuiElement* InitGuiFromEnum(char type, int numOfElements, int vertical, int horizontal, int BorderSize)
{
  GuiElement* array = InitGuiElementArray(type, vertical, horizontal);
  for(int i = 0; i < numOfElements; i++)
  {
    //array = PushGuiElement((GuiElement){(GuiRectangle){i, i, 1, 1}, BorderSize, 'b', "", 0, 255, i}, array);
    array = PushGuiElement((GuiElement){(GuiRectangle){i, 0, 1, 1}, BorderSize, 'b', "", 0, 0, 0, 0}, array);
  }
  return array;
}

void GuiListener(void(*handler)(size_t action)); //build later

void GuiDrawTextInBox(char* text, int originX, int originY, int width, int height, Color textColor)
{
    Rectangle box = { originX, originY, width, height };
    Font font = GetFontDefault();
    float maxHeight = (float)height;
    int fontSize;
    for (fontSize = 1; fontSize <= maxHeight; fontSize++) {
        Vector2 textSize = MeasureTextEx(font, text, fontSize, 0);
        if (textSize.y > maxHeight) break;
    }
    fontSize--;

    float maxWidth = (float)width;
    char newText[TextLength(text)+1];
    strcpy(newText, text);
    int i;
    Vector2 textSize; 
    for(i = TextLength(text); i > 0; i--)
    {
      textSize = MeasureTextEx(font, newText, fontSize, 1);
      if(textSize.x < maxWidth) break;
      newText[i-1] = '.';
      newText[i-2] = '.';
      newText[i] = '\0';
    }

    Vector2 origin = { (float)originX, (float)originY };
    origin.x = origin.x + (maxWidth-textSize.x)/2;
    DrawTextEx(font, newText, origin, fontSize, 1, textColor);
}   

void DrawElement(GuiElement ge, GuiElementArrayHeader head)
{
  int ElementHeight;
  int ElementWidth;
  int OriginX;
  int OriginY;
  if(head.type=='g')
  {
    ElementHeight = (GetScreenHeight()/head.horizontal)*ge.r.h;
    ElementWidth = (GetScreenWidth()/head.vertical)*ge.r.w;
    OriginX = (GetScreenWidth()/head.vertical)*ge.r.x;
    OriginY = (GetScreenHeight()/head.horizontal)*ge.r.y;
  }
  else if(head.type=='r')
  {
    printf("ERROR: Type r not yet supported %s %d\n", __FILE__, __LINE__);
    exit(EXIT_FAILURE);
  }
  else {
  printf("ERROR: Type of GuiElementArray provided is not supported (g or r valid) %s %d\n", __FILE__, __LINE__);
  exit(EXIT_FAILURE);
  }
  DrawRectangle(OriginX, OriginY, ElementWidth, ElementHeight, ge.borderColor);
  DrawRectangle(OriginX+ElementWidth*(((float)ge.border)/200), OriginY+ElementHeight*(((float)ge.border)/200), ElementWidth*(1.0-((float)ge.border)/100), ElementHeight*(1.0-((float)ge.border)/100), ge.color);
  GuiDrawTextInBox(ge.text, OriginX+ElementWidth*(((float)ge.border)/200), OriginY+ElementHeight*(((float)ge.border)/200),  ElementWidth*(1.0-((float)ge.border)/100), ElementHeight*(1.0-((float)ge.border)/100), BLACK);
}

#endif
