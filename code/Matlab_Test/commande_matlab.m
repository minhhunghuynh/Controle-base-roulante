% =========================================
% Simple test pour l'ADC
% Générateur FSK - Protocole de communication robot
% Paramètres : fp = 1000 Hz, Δf = 200 Hz, Ts = 10 ms
% Bit '1' → f1 = 1200 Hz
% Bit '0' → f0 = 800 Hz
% =========================================

clear; clc;

%% Paramètres FSK
fp   = 1000;
df   = 200;
Ts   = 0.010;
Fs   = 44100;
A0   = 1;

f1 = fp + df;   % 1200 Hz
f0 = fp - df;   % 800 Hz

%% Séquence de bits alternés
n_bits = 12;
bits = mod((0:n_bits-1), 2);   % 0, 1, 0, 1, ...

%% Génération du signal FSK
samples_per_bit = round(Ts * Fs);
t_bit = (0:samples_per_bit-1) / Fs;

signal = [];
for k = 1:n_bits
    if bits(k) == 1
        f = f1;
    else
        f = f0;
    end
    segment = A0 * sin(2 * pi * f * t_bit);
    signal = [signal, segment];   %#ok<AGROW>
end

%% Ajout silence début et fin
delay_sec = 1;
silence = zeros(1, round(delay_sec * Fs));
signal_with_delay = [silence, signal, silence];

%% Axe temporel global
t = (0:length(signal_with_delay)-1) / Fs;

%% Sauvegarde WAV
filename = 'signal_fsk.wav';
audiowrite(filename, signal_with_delay, Fs);
fprintf('Fichier sauvegardé : %s\n', filename);

%% Lecture audio
sound(signal_with_delay, Fs);





