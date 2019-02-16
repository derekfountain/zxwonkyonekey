;; Wonky One Key, a ZX Spectrum game featuring a single control key
;; Copyright (C) 2018 Derek Fountain
;; 
;; This program is free software; you can redistribute it and/or
;; modify it under the terms of the GNU General Public License
;; as published by the Free Software Foundation; either version 2
;; of the License, or (at your option) any later version.
;; 
;; This program is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.
;; 
;; You should have received a copy of the GNU General Public License
;; along with this program; if not, write to the Free Software
;; Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
        
;; I'm putting this in the LEVEL_DATA section, which is below the
;; main code in contended memory. I'm not sure how much SP1 references
;; this data; I might need to move this back up into DATA

SECTION LEVEL_DATA

;;    _                    _   _    _ _____   _____     
;;   | |                  | | | |  | |  __ \ / ____|    
;;   | |     _____   _____| | | |  | | |  | | |  __ ___ 
;;   | |    / _ \ \ / / _ | | | |  | | |  | | | |_ / __|
;;   | |___|  __/\ V |  __| | | |__| | |__| | |__| \__ \
;;   |______\___| \_/ \___|_|  \____/|_____/ \_____|___/
;;                                                      
;;                                                      

;; I could use space from the character set...
PUBLIC _blank
._blank
        defb @00000000
        defb @00000000
        defb @00000000
        defb @00000000
        defb @00000000
        defb @00000000
        defb @00000000
        defb @00000000
        
PUBLIC _grassh
._grassh
        defb @01001010
        defb @01101110
        defb @11001110
        defb @11011110
        defb @11111111
        defb @11111111
        defb @11111111
        defb @11111111

PUBLIC _grassv
._grassv
        defb @11111111
        defb @11111011
        defb @10111111
        defb @11111101
        defb @11111111
        defb @11011111
        defb @11110111
        defb @11111111
        
PUBLIC _platform1
._platform1
        defb @11111111
        defb @01010101
        defb @01100100
        defb @11010010
        defb @10101001
        defb @10100010
        defb @01010010
        defb @10000100

PUBLIC _platform1v
._platform1v
        defb @10010101
        defb @01011101
        defb @01100100
        defb @11010010
        defb @10101001
        defb @10111010
        defb @01110111
        defb @10101101
                
PUBLIC _jumper
._jumper
        defb @11111111
        defb @11111111
        defb @00011000
        defb @01111110
        defb @00011000
        defb @00111100
        defb @00011000
        defb @00000000

PUBLIC _block_platform1
._block_platform1
        defb @11111111
        defb @10101010
        defb @01010101
        defb @10101010
        defb @01010101
        defb @10101010
        defb @01010101
        defb @10101010
        
PUBLIC _block_platform2
._block_platform2
        defb @01010101
        defb @10101010
        defb @01010101
        defb @10101010
        defb @01010101
        defb @10101010
        defb @01010101
        defb @10101010

PUBLIC _block_platform3
._block_platform3
        defb @11111111
        defb @11011011
        defb @10100101
        defb @00100100
        defb @01010010
        defb @00100000
        defb @00001000
        defb @00000000
        
PUBLIC _block_platform4
._block_platform4
        defb @11111111
        defb @10001000
        defb @11111111
        defb @00010001
        defb @11111111
        defb @10001000
        defb @11111111
        defb @00010001

PUBLIC _block_platform5
._block_platform5
        defb @11111111
        defb @10000001
        defb @10000001
        defb @10001001
        defb @10010001
        defb @10000001
        defb @10000001
        defb @11111111

PUBLIC _finish
._finish
        defb @11111111
        defb @10000001
        defb @10111101
        defb @10100101
        defb @10100101
        defb @10111101
        defb @10000001
        defb @11111111

PUBLIC _teleporter
._teleporter
        defb @01000010
        defb @10100101
        defb @01001000
        defb @00110000
        defb @00001100
        defb @00010010
        defb @10100101
        defb @01000010

PUBLIC _door_key
._door_key
        defb @01100000
        defb @10010000
        defb @01100000
        defb @00010000
        defb @00001000
        defb @00010100
        defb @00000010
        defb @00000100

PUBLIC _score_slider_left
._score_slider_left
        defb @00000000
        defb @00000000
        defb @01100000
        defb @11111111
        defb @11111111
        defb @01100000
        defb @00000000
        defb @00000000

PUBLIC _score_slider_right
._score_slider_right
        defb @00000000
        defb @00000000
        defb @00000110
        defb @11111111
        defb @11111111
        defb @00000110
        defb @00000000
        defb @00000000

PUBLIC _score_slider_centre
._score_slider_centre
        defb @00000000
        defb @00000000
        defb @00000000
        defb @11111111
        defb @11111111
        defb @00000000
        defb @00000000
        defb @00000000

;;     _____ _                  _                       _____ _ _ _ 
;;    / ____| |                | |                     |  __ (_| | |
;;   | (___ | | _____      ____| | _____      ___ __   | |__) _| | |
;;    \___ \| |/ _ \ \ /\ / / _` |/ _ \ \ /\ / | '_ \  |  ___| | | |
;;    ____) | | (_) \ V  V | (_| | (_) \ V  V /| | | | | |   | | | |
;;   |_____/|_|\___/ \_/\_/ \__,_|\___/ \_/\_/ |_| |_| |_|   |_|_|_|
;;                                                                  
;;                                                                  
     
        defb @00000000
        defb @00000000
        defb @00000000
        defb @00000000
        defb @00000000
        defb @00000000
        defb @00000000

PUBLIC _slowdown_pill_f1
._slowdown_pill_f1
        defb @00000000
        defb @00000000
        defb @00000000
        defb @00000000
        defb @00010000
        defb @00000000
        defb @00000000
        defb @00000000

        defb @00000000
        defb @00000000
        defb @00000000
        defb @00000000
        defb @00000000
        defb @00000000
        defb @00000000
        defb @00000000

PUBLIC _slowdown_pill_f2
._slowdown_pill_f2
        defb @00000000
        defb @00000000
        defb @00000000
        defb @00011000
        defb @00011000
        defb @00000000
        defb @00000000
        defb @00000000

        defb @00000000
        defb @00000000
        defb @00000000
        defb @00000000
        defb @00000000
        defb @00000000
        defb @00000000
        defb @00000000

PUBLIC _slowdown_pill_f3
._slowdown_pill_f3
        defb @00000000
        defb @00000000
        defb @00011000
        defb @00100100
        defb @00100100
        defb @00011000
        defb @00000000
        defb @00000000

        defb @00000000
        defb @00000000
        defb @00000000
        defb @00000000
        defb @00000000
        defb @00000000
        defb @00000000
        defb @00000000



;;    _____
;;   |  __ \
;;   | |  | | ___   ___  _ __
;;   | |  | |/ _ \ / _ \| '__|
;;   | |__| | (_) | (_) | |
;;   |_____/ \___/ \___/|_|
;;
;;

PUBLIC _door_f1
._door_f1
        defb @01111110
        defb @11111111
        defb @11111111
        defb @11111111
        defb @11111111
        defb @11011011
        defb @11011011
        defb @11000011

        defb @00000000
        defb @00000000
        defb @00000000
        defb @00000000
        defb @00000000
        defb @00000000
        defb @00000000
        defb @00000000



;;    ____
;;   |  _ \
;;   | |_) | ___  _ __  _   _ ___
;;   |  _ < / _ \| '_ \| | | / __|
;;   | |_) | (_) | | | | |_| \__ \
;;   |____/ \___/|_| |_|\__,_|___/
;;
;;

PUBLIC _bonus
._bonus
        defb @00000000
        defb @00111100
        defb @01111110
        defb @01100110
        defb @01100110
        defb @01111110
        defb @00111100
        defb @00000000

        defb @00000000
        defb @00000000
        defb @00000000
        defb @00000000
        defb @00000000
        defb @00000000
        defb @00000000
        defb @00000000




;;     _____                          _ _     _
;;    / ____|                        | (_)   | |
;;   | (___   ___ ___  _ __ ___   ___| |_  __| | ___ _ __
;;    \___ \ / __/ _ \| '__/ _ \ / __| | |/ _` |/ _ | '__|
;;    ____) | (_| (_) | | |  __/ \__ | | | (_| |  __| |
;;   |_____/ \___\___/|_|  \___| |___|_|_|\__,_|\___|_|
;;
;;
PUBLIC _score_slider
._score_slider
        defb @01111000
        defb @10110100
        defb @00110000
        defb @00110000
        defb @00110000
        defb @00110000
        defb @10110100
        defb @01111000

        defb @00000000
        defb @00000000
        defb @00000000
        defb @00000000
        defb @00000000
        defb @00000000
        defb @00000000
        defb @00000000


;;      ______          _
;;     |  ____|        | |
;;     | |__ ___  _ __ | |_
;;     |  __/ _ \| '_ \| __|
;;     | | | (_) | | | | |_
;;     |_|  \___/|_| |_|\__|
;;

;; The font is called "Mega 1", and is presented by Einar Saukas‎:
;;  https://sites.google.com/site/zxgraph/home/einar-saukas/fonts
;;
;; It's not clear who owns it. I took it from the ZX-ALFA package:
;;  https://spectrumcomputing.co.uk/index.php?cat=96&id=0025283
;; which claims it's sourced from commercial games.
;;
;; Well, Einar, or whoever owns it, thanks! :)

PUBLIC _font

._font

   BINARY "font.fnt"
