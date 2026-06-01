% =========================================
% Générateur FSK - Commande AVANCER
% Protocole : [start | ordre | sens | param(8bits) | parité]
% Ordre = 1 (avancer)
% Sens  = 1 (avant) | 0 (arrière)
% fp = 1000 Hz, Δf = 200 Hz, Ts = 10 ms
% Bit '1' → f1 = 1200 Hz
% Bit '0' → f0 = 800 Hz
% =========================================
clear; clc;

%% Paramètres FSK
fp  = 1000;
df  = 200;
Ts  = 0.010;
Fs  = 44100;
A0  = 1;
f1  = fp + df;   % 1200 Hz
f0  = fp - df;   % 800 Hz

%% Saisie utilisateur
distance_cm = input('Distance à parcourir (0 – 255 cm) : ');
sens        = input('Sens  [1 = avant  |  0 = arrière]  : ');

%% Construction de la trame
start_bit  = 0;
ordre_bit  = 1;                                % 1 = avancer
sens_bit   = sens;
param_bits = bitget(distance_cm, 8:-1:1);   % MSB en premier, sans toolbox

% Parité paire
data_bits  = [start_bit, ordre_bit, sens_bit, param_bits];
parite_bit = mod(sum(data_bits), 2);

bits = [start_bit, ordre_bit, sens_bit, param_bits, parite_bit];

%% Affichage de la trame
fprintf('\n=== Trame générée ===\n');
fprintf('  Start  : %d\n',              bits(1));
fprintf('  Ordre  : %d  (avancer)\n',   bits(2));
if sens == 1
    fprintf('  Sens   : %d  (avant)\n',   bits(3));
else
    fprintf('  Sens   : %d  (arrière)\n', bits(3));
end
fprintf('  Param  : %d%d%d%d%d%d%d%d  (%d cm)\n', bits(4:11), distance_cm);
fprintf('  Parité : %d\n',              bits(12));
fprintf('  Trame  : ');  fprintf('%d', bits);  fprintf('\n\n');

%% Génération du signal FSK
n_bits          = length(bits);
samples_per_bit = round(Ts * Fs);
t_bit           = (0:samples_per_bit-1) / Fs;
signal          = [];

for k = 1:n_bits
    if bits(k) == 1
        f = f1;
    else
        f = f0;
    end
    segment = A0 * sin(2 * pi * f * t_bit);
    signal  = [signal, segment];   %#ok<AGROW>
end

%% Ajout silence début et fin
delay_sec         = 1;
silence           = zeros(1, round(delay_sec * Fs));
signal_with_delay = [silence, signal, silence];

%% Sauvegarde WAV
if sens == 1
    filename = sprintf('avancer_%dcm_avant.wav', distance_cm);
else
    filename = sprintf('avancer_%dcm_arriere.wav', distance_cm);
end
audiowrite(filename, signal_with_delay, Fs);
fprintf('Fichier sauvegardé : %s\n', filename);

%% Lecture audio
sound(signal_with_delay, Fs);

%% Tracé du signal (zoom sur la trame, sans les silences)
t_signal = (0:length(signal)-1) / Fs * 1000;   % en ms
figure('Name', 'FSK – Avancer');
plot(t_signal, signal);
xlabel('Temps (ms)');  ylabel('Amplitude');
if sens == 1
    title(sprintf('Commande Avancer – %d cm – Avant', distance_cm));
else
    title(sprintf('Commande Avancer – %d cm – Arrière', distance_cm));
end
grid on;