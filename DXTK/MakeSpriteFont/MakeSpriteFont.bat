@echo off

REM ASCII 0 ~ 255 + Æ¯¼ö¹®ÀÚ'¡á' + Unicode ÇÑ±Û ¿Ï¼ºÇü ÃÑ 11,440 ±ÛÀÚ
MakeSpriteFont "±¼¸²" myfile.sfont /FontSize:9 /CharacterRegion:0x0-0xff /CharacterRegion:0x25a0 /CharacterRegion:0xAC00-0xD7AF  /DefaultCharacter:0x5f /FastPack


REM MakeSpriteFont "±¼¸²" myfile.sfont /FontSize:10
REM MakeSpriteFont "¸¼Àº °íµñ" myfile.sfont /FontSize:10
REM MakeSpriteFont "Consolas" myfile.sfont /FontSize:10
