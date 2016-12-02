#!/bin/bash

    # wczytanie danych wejściowych
    last_ident=""
    n=10; k=100; s=1; p1=""; p2=""; ai1=""; ai2=""
    x1=""; x2=""; y1=""; y2=""
    for elem in $@; do
        case "$last_ident" in
            "-n" ) n=${elem}; last_ident="";;
            "-k" ) k=${elem}; last_ident="";;
            "-s" ) s=${elem}; last_ident="";;
            "-p1" ) p1=${elem}; last_ident="";;
            "-p2" ) p2=${elem}; last_ident="";;
            "-ai1" ) ai1=${elem}; last_ident="";;
            "-ai2" ) ai2=${elem}; last_ident="";;
            "" ) last_ident=${elem};;
            * ) exit 1;;
        esac
        if [ "$last_ident" != "-n" ] && [ "$last_ident" != "-k" ] &&
        [ "$last_ident" != "-s" ] && [ "$last_ident" != "-p1" ] &&
        [ "$last_ident" != "-p2" ] && [ "$last_ident" != "-ai1" ] &&
        [ "$last_ident" != "-ai2" ] && [ "$last_ident" != "" ]; then exit 1; fi
    done
    if [ "$last_ident" != "" ]; then exit 1; fi
    if ! [ "$n" -eq "$n" ] 2>/dev/null || (( "$n" > "2147483647" )); then
        exit 1;
    fi
    check_p1() {
        if ! [ "$x1" -eq "$x1" ] 2>/dev/null; then exit 1; fi
        if ! [ "$y1" -eq "$y1" ] 2>/dev/null; then exit 1; fi
    }
    check_p2() {
        if ! [ "$x2" -eq "$x2" ] 2>/dev/null; then exit 1; fi
        if ! [ "$y2" -eq "$y2" ] 2>/dev/null; then exit 1; fi
    }
    if [ "$p1" != "" ] && [ "$p2" != "" ]; then
        # parsowanie współrzędnych królów
        IFS=',' read -r -a array <<< "$p1"
        x1=${array[0]}; y1=${array[1]}
        IFS=',' read -r -a array <<< "$p2"
        x2=${array[0]}; y2=${array[1]}
        check_p1; check_p2
    else
        # losowanie pozycji królów
        if [ "$p1" == "" ] && [ "$p2" == "" ]; then
            while true; do
                x1=$(($RANDOM % $[n - 3] + 1))
                y1=$(($RANDOM % $n + 1))
                x2=$(($RANDOM % $[n - 3] + 1))
                y2=$(($RANDOM % $n + 1))
                if [ $[$y1 - $y2] -ge 8 ] || [ $[$y2 - $y1] -ge 8 ] ||
                [ $[$x1 - $x2] -ge 8 ] || [ $[$x2 - $x1] -ge 8 ]; then break; fi
            done
        else
            if [ "$p2" == "" ]; then
                IFS=',' read -r -a array <<< "$p1"
                x1=${array[0]}; y1=${array[1]}
                x2=${x1}; y2=${y1}
            else
                IFS=',' read -r -a array <<< "$p2"
                x2=${array[0]}; y2=${array[1]}
            fi
            check_p2
            echo "$x1 $y1"
            for i in `seq 1 1000`; do
                x1=$(($RANDOM % $[n - 3] + 1))
                y1=$(($RANDOM % $n + 1))
                if [ $[$y1 - $y2] -ge 8 ] || [ $[$y2 - $y1] -ge 8 ] ||
                [ $[$x1 - $x2] -ge 8 ] || [ $[$x2 - $x1] -ge 8 ]; then break; fi
                if [ "$i" == "1000" ]; then exit 1; fi
            done
        fi
    fi

    # utworzenie deskryptorów
    fd3=$(mktemp -u); mkfifo ${fd3}; exec 3<>${fd3}
    fd4=$(mktemp -u); mkfifo ${fd4}; exec 4<>${fd4}
    fd5=$(mktemp -u); mkfifo ${fd5}; exec 5<>${fd5}
    fd6=$(mktemp -u); mkfifo ${fd6}; exec 6<>${fd6}
    fd7=$(mktemp -u); mkfifo ${fd7}; exec 7<>${fd7}
    fd8=$(mktemp -u); mkfifo ${fd8}; exec 8<>${fd8}

    # tworzenie funkcji nadzorujących
    rm -f game.err; rm -f ai1.err; rm -f ai2.err
    start_game() {
     echo "$1"
        ./sredniowiecze_gui_with_libs.sh $1 $2 0<${fd3} >&8 &
        GAME_PID=$!
        wait ${GAME_PID}
        echo "$?" >game.err
    }
    start_ai1() {
        ${ai1} 0<${fd4} >&5 &
        AI1_PID=$!
        wait ${AI1_PID}
        echo "$?" >ai1.err
    }
    start_ai2() {
        ${ai2} 0<${fd6} >&7 &
        AI2_PID=$!
        wait ${AI2_PID}
        echo "$?" >ai2.err
    }

    # utworzenie init-ów
    INIT1="INIT $n $k 1 $x1 $y1 $x2 $y2"
    INIT2="INIT $n $k 2 $x1 $y1 $x2 $y2"

    # sprawdzenie trybu gry i rozpoczęcie gry
    tryb="0";
    if [ "$ai1" != "" ] && [ "$ai2" != "" ]; then
        tryb="2"
        start_game "" "" &
        start_ai1 &
        start_ai2 &
        sleep 0.01
        printf "$INIT1\n" >&3; printf "$INIT1\n" >&4
        printf "$INIT2\n" >&3; printf "$INIT2\n" >&6
    elif [ "$ai1" == "" ] && [ "$ai2" == "" ]; then
        tryb="0"
        start_game "-human1" "-human2" &
        sleep 0.01
        printf "$INIT1\n$INIT2\n" >&3
    elif [ "$ai1" != "" ]; then
        tryb="1a"
        start_game "-human2" &
        start_ai1 &
        sleep 0.01
        printf "$INIT1\n$INIT2\n" >&3;
        printf "$INIT1\n" >&4
    else
        tryb="1b"
        start_game "-human1" &
        ai1=${ai2}
        start_ai1 &
        sleep 0.01
        printf "$INIT1\n$INIT2\n" >&3;
        printf "$INIT2\n" >&4
    fi;
    kill_all() {
        kill ${GAME_PID} 2> /dev/null
        kill ${AI1_PID} 2> /dev/null
        kill ${AI2_PID} 2> /dev/null
    }
    dead_or_alive() {
        if [ -a ai1.err ] && [ -a ai2.err ]; then
            a2=(`cat ai1.err`); a3=(`cat ai2.err`)
            kill_all
            if [ "${a2}" != "0" ] && [ "${a2}" != "1" ] && [ "${a2}" != "2" ]; then exit 1
            elif [ "${a3}" != "0" ] && [ "${a3}" != "1" ] && [ "${a3}" != "2" ]; then exit 1;
            else exit 0; fi
        fi
        if [ -a ai1.err ]; then
            a2=(`cat ai1.err`);
            kill_all
            if [ "${a2}" != "0" ] && [ "${a2}" != "1" ] && [ "${a2}" != "2" ]; then exit 1; fi
        fi
        if [ -a ai3.err ]; then
            a3=(`cat ai2.err`);
            kill_all
            if [ "${a3}" != "0" ] && [ "${a3}" != "1" ] && [ "${a3}" != "2" ]; then exit 1; fi
        fi
        if [ -a game.err ]; then
            a1=(`cat game.err`)
            kill_all
            if [ "${a1}" != "0" ]; then exit 1; else exit 0; fi
        fi
    }

    tryb1_ai() {
        dead_or_alive
        printf "" >"g1.in"; printf "" >"g2.in"
        line=""
        while [ "$line" != "END_TURN" ] && [ "$line" != "END_TURN" ] && read -t 1 <&5 line; do
            echo "$line" >>"g1.in"
        done
        if [ "$line" != "END_TURN" ] && [ "$line" != "" ]; then
            exit 1
        fi
        cat "g1.in" >&3;
        dead_or_alive
    }

    tryb1_human() {
        dead_or_alive
        line=""
        while [ "$line" != "END_TURN" ]; do
            while true; do
                read -t 1 <&8 line
                if [ "${line}" != "" ]; then
                    echo "${line}" >>"g2.in"
                else
                    break
                fi
                if [ "$line" == "END_TURN" ]; then
                    break
                fi
            done
            break
        done
        cat "g2.in" >&4
        if [ "$line" != "END_TURN" ] && [ "$line" != "" ]; then
            exit 1
        fi
        if [ -a game.err ]; then
            sleep 1.6
        fi
        dead_or_alive
    }

    timestamp() {
        date
    }
    echo "" > log.txt

    tryb2_ai1() {
        printf "" >"g1.in";
        #dead_or_alive
        line=""
        while [ "$line" != "END_TURN" ] && read -t 1 <&5 line; do
            echo "$line" >>"g1.in"
        done
#        if [ "$line" != "END_TURN" ]; then
#            exit 1
#        fi
        sleep ${s}
        cat "g1.in" >&6; cat "g1.in" >&3

#        timestamp
#        echo "ai1" >> log.txt
#        cat "g1.in" >> log.txt
        dead_or_alive
    }

    tryb2_ai2() {
        printf "" >"g2.in"
        line=""
        while [ "$line" != "END_TURN" ] && read -t 1 <&7 line; do
            echo "$line"  >>"g2.in"
        done
#        if [ "$line" != "END_TURN" ]; then
#            exit 1
#        fi
        sleep ${s}
        cat "g2.in" >&4; cat "g2.in" >&3
#        timestamp
#        echo "ai2" >> log.txt
#        cat "g2.in" >> log.txt
        dead_or_alive
    }

    rm -f g1.in; rm -f g2.in
    case "$tryb" in
        "0" )
            while true; do
                dead_or_alive
            done
            ;;
        "1a" )
            while true; do
                printf "" >"g1.in"; printf "" >"g2.in"
                tryb1_ai
                tryb1_human
            done
            ;;
        "1b" )
            while true; do
                printf "" >"g1.in"; printf "" >"g2.in"
                tryb1_human
                tryb1_ai
            done
            ;;
        "2" )
            while true; do
                tryb2_ai1
                tryb2_ai2
            done
            ;;
    esac

    rm -f g1.in; rm -f g2.in
    rm -f game.err; rm -f ai1.err; rm -f ai2.err